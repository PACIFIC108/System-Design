#include<bits/stdc++.h>
using namespace std;

void INIT() {
	freopen("input.txt" , "r" , stdin);
	freopen("output.txt" , "w" , stdout);
}

class FullSlotException: public exception {
public:
	const char* what() const noexcept override {
		return "Parking Slot is Full";
	}
};

class InvalidTicketException: public exception {
public:
	const char* what() const noexcept override {
		return "Parking Ticket is Invalid";
	}
};

enum class SlotType {
	CAR, BUS, BIKE
};
class Slot;
class Vehicle;
class Ticket {
	string id;
	int entryHours;
	Slot* s;
	Vehicle* v;
public:
	Ticket(string id, int entryHours, Slot* s, Vehicle* v): id(id), entryHours(entryHours), s(s), v(v) {}
	string getId() {return id;}
	int getHours() {return entryHours;}
	Slot* getSlot() {return s;}
	Vehicle* getVehicle() {return v;}
};

class Slot {
	SlotType type;
	bool equipped = false;
public:
	Slot(SlotType type): type(type) {}
	SlotType getType() {return type;}
	void setEquipped(bool status) {equipped = status;}
	bool isEquipped() {return equipped;}
};

class Floor {
	int id;
	vector<Slot> slots;
public:
	Floor(int id, vector<Slot> slots): id(id), slots(slots) {}

	Slot* AvailableSlot(SlotType type) {
		// FIX: Added '&' to modify the actual slot in the vector, not a copy
		for (auto& i : slots) {
			if (!i.isEquipped() && i.getType() == type) {
				i.setEquipped(true);
				return &i;
			}
		}
		return nullptr;
	}
	int getId() {return id;}
};

class Vehicle {
protected:
	string name, model;
	Ticket* t;
public:
	// FIX: Ticket is initially null because it hasn't entered the gate yet
	Vehicle(string name, string model): name(name), model(model), t(nullptr) {}

	void setTicket(Ticket* ticket) { t = ticket; }
	Ticket* getTicket() { return t; }

	virtual string getName() = 0;
	virtual string getModel() = 0;
	virtual SlotType getVehicleType() = 0;
	virtual ~Vehicle() {}
};

class Car: public Vehicle {
public:
	Car(string name, string model): Vehicle(name, model) {}
	string getName() override {return name;}
	string getModel() override {return model;}
	SlotType getVehicleType() override { return SlotType::CAR; }
};

class Bus: public Vehicle {
public:
	Bus(string name, string model): Vehicle(name, model) {}
	string getName() override {return name;}
	string getModel() override {return model;}
	SlotType getVehicleType() override { return SlotType::BUS; }
};

class Bike: public Vehicle {
public:
	Bike(string name, string model): Vehicle(name, model) {}
	string getName() override {return name;}
	string getModel() override {return model;}
	SlotType getVehicleType() override { return SlotType::BIKE; }
};

class PaymentMethods {
protected:
	string name;
public:
	PaymentMethods(string name): name(name) {}
	virtual void action(int price) = 0;
	virtual ~PaymentMethods() {}
};

class UPI: public PaymentMethods {
public:
	UPI(string name): PaymentMethods(name) {}
	void action(int price) override {
		cout << "Payment of RS. " << price << " is done via " << name << endl;
	}
};

class Card: public PaymentMethods {
public:
	Card(string name): PaymentMethods(name) {}
	void action(int price) override {
		cout << "Payment of RS. " << price << " is done via " << name << endl;
	}
};

class Cash: public PaymentMethods {
public:
	Cash(string name): PaymentMethods(name) {}
	void action(int price) override {
		cout << "Payment of RS. " << price << " is done via " << name << endl;
	}
};

class PricingStrategy {
protected:
	int rate;
public:
	PricingStrategy(int rate): rate(rate) {}
	virtual int totalPrice(int entryHour, int exitHour) = 0;
	virtual ~PricingStrategy() {}
};

class Hourly: public PricingStrategy {
public:
	Hourly(int p): PricingStrategy(p) {}
	int totalPrice(int entryHour, int exitHour) override {
		int duration = exitHour - entryHour;
		if (duration <= 0) duration = 1; // Minimum 1 hour charge
		return duration * rate;
	}
};

class Fixed: public PricingStrategy {
public:
	Fixed(int p): PricingStrategy(p) {}
	int totalPrice(int entryHour, int exitHour) override {
		return rate; // Flat fee regardless of time
	}
};

class ParkingLot {
	vector<Floor> floors;
public:
	ParkingLot(vector<Floor> f): floors(f) {}

	Slot* findAvailableSlot(SlotType type) {
		for (auto& floor : floors) {
			Slot* s = floor.AvailableSlot(type);
			if (s != nullptr) return s;
		}
		return nullptr;
	}
};

class EntryGate {
	ParkingLot& lot;
	int ticketCounter = 1;
public:
	EntryGate(ParkingLot& lot) : lot(lot) {}

	Ticket* processEntry(Vehicle* v, int currentHour) {
		Slot* slot = lot.findAvailableSlot(v->getVehicleType());
		if (!slot) {
			cout << "Lot is full for vehicle : " << v->getName() << endl;
			throw FullSlotException();
		}

		string tId = "TKT - " + to_string(ticketCounter++);
		Ticket* t = new Ticket(tId, currentHour, slot, v);
		v->setTicket(t);

		cout << v->getName() << " entered. Ticket " << tId << " generated.\n";
		return t;
	}
};

class ExitGate {
	PricingStrategy* pricing;
public:
	ExitGate(PricingStrategy* p) : pricing(p) {}

	void processExit(Ticket* t, int exitHour, PaymentMethods* payment) {
		// Ticket* t = v->getTicket();
		if (!t) {
			throw InvalidTicketException();
		}

		// 1. Calculate Price
		int finalPrice = pricing->totalPrice(t->getHours(), exitHour);

		// 2. Process Payment (Using UPI dynamically for this example)
		// PaymentMethods* payment = new UPI(finalPrice);
		payment->action(finalPrice);

		// 3. Free the Spot
		Slot* assignedSlot = t->getSlot();
		assignedSlot->setEquipped(false);
		cout << t->getVehicle()->getName() << " exited safely. Slot freed.\n\n";

		// Cleanup memory
		delete payment;
		t->getVehicle()-> setTicket(nullptr);
		delete t;
	}
};


int main() {
	INIT();

	// 1. Setup Slots and Floors
	vector<Slot> floor1Slots = { Slot(SlotType::CAR), Slot(SlotType::BIKE) };
	Floor f1(1, floor1Slots);
	vector<Floor> allFloors = { f1 };

	// 2. Initialize Core Systems
	ParkingLot myLot(allFloors);
	EntryGate entryGate(myLot);
	PricingStrategy* hourlyPricing = new Hourly(50); // Rs. 50 per hour
	ExitGate exitGate(hourlyPricing);

	// 3. Create Vehicles
	Car myCar("Honda Civic", "2022");
	Bike myBike("Yamaha R15", "2020");
	Car mySecondCar("Toyota Corolla", "2021");

	// 4. Run the Simulation
	cout << "-- - ENTRY PHASE -- -\n";
	Ticket* carT = nullptr;
	Ticket* bikeT = nullptr;
	Ticket* secondCarT = nullptr;
	try {
		carT = entryGate.processEntry(&myCar, 10);  // Enters at 10:00
		bikeT = entryGate.processEntry(&myBike, 11); // Enters at 11:00

		// This should fail because the floor only has 1 car slot and 1 bike slot!
		secondCarT = entryGate.processEntry(&mySecondCar, 11);
	} catch (const exception &e) {
		cout << "Error is: " << e.what() << endl;
	}

	cout << "\n-- - EXIT PHASE -- -\n";
	try {
		exitGate.processExit(carT, 14, new UPI("PayTM")); // Exits at 14:00 (4 hours)
		exitGate.processExit(bikeT, 12, new Cash("INR")); // Exits at 12:00 (1 hour)
		exitGate.processExit(secondCarT, 15, new Cash("INR")); // Exits at 12:00 (1 hour)
	} catch (const exception &e) {
		cout << "Error is: " << e.what() << endl;
	}
	delete hourlyPricing;

	return 0;
}
