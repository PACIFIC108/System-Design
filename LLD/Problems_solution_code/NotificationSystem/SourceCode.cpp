#include<bits/stdc++.h>
using namespace std;


void INIT() {
	freopen("input.txt" , "r" , stdin);
	freopen("output.txt" , "w" , stdout);
}


class User {
	string name, email, phone;
public:
	User(string name, string email, string phone): name(name), email(email), phone(phone) {}
	string getName() {return name;}
	string getPhone() {
		return phone;
	}
	string getEmail() {return email;}
	~User() {}
};

class NotificationType {
protected:
	string message;
public:
	NotificationType(string message): message(message) {}
	virtual void send(User* user) = 0;
	virtual ~NotificationType() {}
};
class SMS: public NotificationType {
public:
	SMS(string message): NotificationType( message) {}
	void send(User* user)override {
		cout << "Sending SMS to " << user->getPhone() << ": " << message << endl;
	}
};
class Email: public NotificationType {
public:
	Email(string message): NotificationType( message) {}
	void send(User* user)override {
		cout << "Sending Email to " << user->getEmail() << ": " << message << endl;
	}
};
class Push: public NotificationType {
public:
	Push(string message): NotificationType( message) {}
	void send(User* user)override {
		cout << "Sending Push to " << user->getName() << ": " << message << endl;
	}
};

class NotificationFactory {
public:
	virtual NotificationType* create(string message) = 0;
	virtual ~NotificationFactory() {}
};
class SMSFactory: public NotificationFactory {
public:
	NotificationType* create(string message) override {return new SMS(message);}
};
class EmailFactory: public NotificationFactory {
public:
	NotificationType* create(string message) override {return new Email(message);}
};
class PushFactory: public NotificationFactory {
public:
	NotificationType* create(string message) override {return new Push(message);}
};

class Notification {
	string message;
	NotificationFactory* NF;
	User* user;
	mutex mtx;
public:
	Notification(string message, NotificationFactory* NF, User* user): message(message), NF(NF), user(user) {}
	void createNotification() {
		lock_guard<mutex> lock(mtx);
		NotificationType* type = NF->create(message);
		type->send(user);
		delete type;
	}
	~Notification() {
	}
};

int main() {
	INIT();
	// ------  Write Your Code From Here -----------
	NotificationFactory* SMS = new SMSFactory();
	NotificationFactory* Push = new PushFactory();
	NotificationFactory* Email = new EmailFactory();
	User* user = new User("Prashant", "pkkr.pacific@gmail.com", "6201663796");
	Notification notification1("Create a Notification system", SMS, user);
	Notification notification2("Create a Notification system", Email, user);
	Notification notification3("Create a Notification system", Push, user);
	notification1.createNotification();
	notification2.createNotification();
	notification3.createNotification();
	delete user;
	delete SMS;
	delete Push;
	delete Email;

	return 0;
}
