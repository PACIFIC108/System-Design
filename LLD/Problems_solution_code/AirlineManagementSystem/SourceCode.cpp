#include<bits/stdc++.h>
using namespace std;


void INIT() {
	freopen("input.txt" , "r" , stdin);
	freopen("output.txt" , "w" , stdout);
}

// ================= Exceptions =================
class NoSeatFoundException : public exception {
public:
	const char* what() const noexcept override { return "No seat found"; }
};

class SeatAlreadyBookedException : public exception {
public:
	const char* what() const noexcept override { return "Seat is already booked by someone else"; }
};

class NoFlightFoundException : public exception {
public:
	const char* what() const noexcept override { return "No Flight found"; }
};

class NoTicketFoundException : public exception {
public:
	const char* what() const noexcept override { return "No Ticket found"; }
};

// ================= Entities =================
class User {
protected:
	string name, phone, email;
public:
	User(string name, string phone, string email): name(name), phone(phone), email(email) {}
	string getName() { return name; }
	string getPhone() { return phone; }
	string getEmail() { return email; }
	virtual ~User() {}
};

class Passanger : public User {
	string luggageNo;
	int weight;
public:
	Passanger(string name, string phone, string email, string luggageNo, int weight):
		User(name, phone, email), luggageNo(luggageNo), weight(weight) {}

	string getLuggageNo() { return luggageNo; }
	int getWeight() { return weight; }
};

class Seat {
	string seatNumber;
	bool isBooked = false; // FIX: Added state!
public:
	Seat(string seatNumber): seatNumber(seatNumber) {}
	string getSeatNumber() { return seatNumber; }

	// Seat State Methods
	bool getIsBooked() { return isBooked; }
	void markAsBooked() { isBooked = true; }
	void markAsAvailable() { isBooked = false; }
};

class Flight {
	string name, id, source, destination;
	vector<Seat*> seats;
	int base_fare;
	int hours;
public:
	Flight(string name, string id, string source, string destination, int base_fare, int hours):
		name(name), id(id), source(source), destination(destination), base_fare(base_fare), hours(hours) {}

	void addSeat(Seat* seat) { seats.push_back(seat); }
	string getId() { return id; }
	string getName() { return name; }
	string getSource() { return source; }
	string getDestination() { return destination; }
	int getFare() { return hours * base_fare; }

	Seat* getSeat(string seatNumber) {
		for (auto seat : seats) {
			if (seat->getSeatNumber() == seatNumber) {
				return seat;
			}
		}
		throw NoSeatFoundException();
	}
};

class Ticket {
	string pnr;
	Flight* flight;
	Seat* seat;
	Passanger* user;
public:
	Ticket(string pnr, Flight* flight, Seat* seat, Passanger* user):
		pnr(pnr), flight(flight), seat(seat), user(user) {}

	string getPNR() { return pnr; }
	string getFlightName() { return flight->getName(); }
	string getSeatNumber() { return seat->getSeatNumber(); }
	string getUserName() { return user->getName(); }
	int fare() { return flight->getFare(); }

	Seat* getSeatObject() { return seat; } // Needed for cancellation
	~Ticket() {}
};

// ================= Payment (Strategy) =================
class PaymentMethod {
public:
	virtual void pay(int amount) = 0;
	virtual ~PaymentMethod() {}
};

class UPI : public PaymentMethod {
public:
	void pay(int amount) override {
		cout << "Paid " << amount << " using UPI\n";
	}
};

class CreditCard : public PaymentMethod {
public:
	void pay(int amount) override {
		cout << "Paid " << amount << " using Credit Card\n";
	}
};

// ================= Booking Logic =================
class Booking {
public:
	Ticket* bookTicket(PaymentMethod* payment, Flight* flight, Seat* seat, Passanger* user) {
		// 1. Check if seat is actually available!
		if (seat->getIsBooked()) {
			throw SeatAlreadyBookedException();
		}

		// 2. Calculate final price
		//we can have PricingStrategy and apply it here but for now we will keep it simple
		int extraCharge = 0;
		if (user->getWeight() > 15) {
			extraCharge = (user->getWeight() - 15) * 100;
		}
		int totalFare = flight->getFare() + extraCharge;

		// 3. Retry Logic
		int maxRetries = 3;
		int attempt = 0;
		bool paymentSuccess = false;

		while (attempt < maxRetries && !paymentSuccess) {
			try {
				payment->pay(totalFare);
				paymentSuccess = true;
			} catch (const exception& e) {
				attempt++;
				cout << "[Warning] Payment failed: " << e.what()
				     << " | Retrying (" << attempt << "/" << maxRetries << ")...\n";

				if (attempt >= maxRetries) {
					cout << "[Error] Max payment attempts reached. Transaction aborted.\n";
					return nullptr;
				}
				this_thread::sleep_for(chrono::seconds(2));
			}
		}

		// 4. Success! Generate ticket and mark seat as taken
		string pnr = "PNR" + to_string(rand() % 10000);
		seat->markAsBooked();
		Ticket* ticket = new Ticket(pnr, flight, seat, user);

		return ticket;
	}

	// FIX: Using reference (&) so we actually modify the main array
	int getRefundAmount(string pnr, vector<Ticket*>& tickets) {
		for (auto it = tickets.begin(); it != tickets.end(); ++it) {
			if ((*it)->getPNR() == pnr) {
				int price = (*it)->fare() * 0.8;

				// Free the seat so someone else can buy it!
				(*it)->getSeatObject()->markAsAvailable();

				delete *it;            // Delete from memory
				tickets.erase(it);     // Remove from array (prevents double-free crash)

				return price;
			}
		}
		throw NoTicketFoundException();
	}
};

// ================= Facade =================
class AirlineManagementSystem {
	vector<Flight*> flights;
	vector<Ticket*> tickets;
public:
	AirlineManagementSystem(vector<Flight*> flights): flights(flights) {}

	Flight* getFlight(string id) {
		for (auto flight : flights) {
			if (flight->getId() == id) {
				return flight;
			}
		}
		throw NoFlightFoundException();
	}

	string BookFlight(string flightId, string seatNumber, Passanger* user, PaymentMethod* payment) {
		Flight* flight = getFlight(flightId);
		Seat* seat = flight->getSeat(seatNumber);
		Booking* booking = new Booking();

		Ticket* ticket = nullptr;
		try {
			ticket = booking->bookTicket(payment, flight, seat, user);
		} catch (const exception& e) {
			cout << "Booking failed: " << e.what() << endl;
			delete booking;
			return "";
		}

		if (!ticket) {
			cout << "Booking failed try again later\n";
			delete booking;
			return "";
		}

		tickets.push_back(ticket);
		cout << "Ticket booked successfully. PNR: " << ticket->getPNR() << "\n\n";

		delete booking;
		// DO NOT delete user here, the ticket needs to read their name!
		// DO NOT delete payment here, usually handled by whoever created it.
		return ticket->getPNR();
	}

	void cancelTicket(string pnr) {
		int refundAmount = 0;
		Booking* booking = new Booking();
		try {
			refundAmount = booking->getRefundAmount(pnr, tickets);
			cout << "Ticket " << pnr << " cancelled successfully. Refund amount: " << refundAmount << "\n\n";
		} catch (const NoTicketFoundException& e) {
			cout << "No ticket found with PNR: " << pnr << endl;
		}
		delete booking;
	}

	~AirlineManagementSystem() {
		for (auto flight : flights) { delete flight; }
		for (auto ticket : tickets) { delete ticket; }
	}
};

class SearchManager {
	unordered_map<string, vector<string>>flightList;
	AirlineManagementSystem* airlineManagementSystem;
public:
	SearchManager(unordered_map<string, vector<string>>flightList, AirlineManagementSystem* airlineManagementSystem):
		flightList(flightList), airlineManagementSystem(airlineManagementSystem) {}

	vector<Flight*> searchFlights(string source, string destination) {
		vector<Flight*> result;
		string key = source + ":" + destination;
		for (auto flight : flightList[key]) {
			try {
				result.push_back(airlineManagementSystem->getFlight(flight));
			} catch (const NoFlightFoundException& e) {
				// Ignore
			}
		}
		if (result.empty()) throw NoFlightFoundException();
		return result;
	}
};

int main() {
	INIT();
	srand(time(0));

	// 1. Setup
	vector<Flight*> flights;
	Seat* seat1 = new Seat("1A");
	Seat* seat2 = new Seat("1B");

	Flight* flight1 = new Flight("Indigo", "6E123", "Delhi", "Mumbai", 5000, 2);
	flight1->addSeat(seat1);
	flight1->addSeat(seat2);

	Flight* flight2 = new Flight("Air India", "AI456", "Delhi", "Mumbai", 6000, 2);
	flight2->addSeat(seat1); // Note: Sharing seat objects across flights is risky in real life, but fine for simulation!

	flights.push_back(flight1);
	flights.push_back(flight2);
	AirlineManagementSystem* ams = new AirlineManagementSystem(flights);

	unordered_map<string, vector<string>> flightList;
	flightList["Delhi:Mumbai"] = {"6E123", "AI456"};
	SearchManager* searchManager = new SearchManager(flightList, ams);

	// 2. Search
	Flight* MinFlight = nullptr;
	vector<Flight*> searchResults;
	try {
		searchResults = searchManager->searchFlights("Delhi", "Mumbai");
	} catch (const NoFlightFoundException& e) {
		cout << "No flights found" << endl;
		return 0;
	}

	for (auto flight : searchResults) {
		if (MinFlight == nullptr || flight->getFare() < MinFlight->getFare()) {
			MinFlight = flight;
		}
	}
	cout << "From Delhi -> Mumbai  \nCheapest Flight: " << MinFlight->getName() << ", ID: " << MinFlight->getId() << "\n\n";

	// 3. Bookings
	Passanger* john = new Passanger("John Doe", "1234567890", "example@gmail.com", "LUG123", 20);
	PaymentMethod* upi = new UPI();

	string PNR = ams->BookFlight(MinFlight->getId(), "1A", john, upi);

	// Try to book the exact same seat again to test our Exception!
	cout << "--- Attempting to double-book Seat 1A ---\n";
	ams->BookFlight(MinFlight->getId(), "1A", john, upi);

	// 4. Cancellations
	ams->cancelTicket("PNR1234"); // Fake PNR
	if (PNR != "") ams->cancelTicket(PNR); // Real PNR

	// 5. Cleanup
	delete ams;
	delete searchManager;
	delete seat1;
	delete seat2;
	delete john;
	delete upi;

	return 0;
}
