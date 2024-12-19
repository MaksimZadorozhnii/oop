#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <memory>
#include <algorithm>

using namespace std;

struct Date {
    int day;
    int month;
    int year;

    Date(int d, int m, int y) : day(d), month(m), year(y) {}
    bool operator==(const Date& other) const {
        return (day == other.day && month == other.month && year == other.year);
    }
    bool operator<(const Date& other) const {
        if (year < other.year) return true;
        if (year > other.year) return false;
        if (month < other.month) return true;
        if (month > other.month) return false;
        return day < other.day;
    }

    bool operator>(const Date& other) const {
        return !(*this < other) && !(*this == other);
    }
};

ostream& operator<<(ostream& os, const Date& date) {
    os << date.day << "/" << date.month << "/" << date.year;
    return os;
}

struct Event {
    Date date;
    string description;

    Event(const Date& d, const string& desc) : date(d), description(desc) {}
};

struct Reminder {
    Date date;
    string message;

    Reminder(const Date& d, const string& msg) : date(d), message(msg) {}
};


// Абстрактный наблюдатель
class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(const Reminder& reminder) = 0;
};

// Конкретные наблюдатели
class ConsoleObserver : public Observer {
public:
    void update(const Reminder& reminder) override {
        cout << "Уведомление в консоли: " << reminder.message << " (" << reminder.date << ")" << endl;
    }
};

class EmailObserver : public Observer {
public:
    void update(const Reminder& reminder) override {
        cout << "Отправка email: " << reminder.message << " (" << reminder.date << ")" << endl;
        // Здесь должна быть реализация отправки email
    }
};


// Субъект (Календарь), который отслеживает наблюдателей
class Calendar {
public:
    void addObserver(Observer* observer) {
        observers.push_back(observer);
    }

    void removeObserver(Observer* observer) {
         observers.erase(remove(observers.begin(), observers.end(), observer), observers.end());
    }

    void addReminder(const Reminder& reminder) {
      reminders.push_back(reminder);
      notify(reminder);
    }

    void addEvent(const Event& event) {
      events.push_back(event);
    }

    vector<Reminder> getRemindersByDate(const Date& date) {
       vector<Reminder> reminders_by_date;
        for(const auto& reminder: reminders) {
            if (reminder.date == date) {
                reminders_by_date.push_back(reminder);
            }
        }
         return reminders_by_date;
    }
    vector<Event> getEventsByDate(const Date& date) {
       vector<Event> events_by_date;
        for(const auto& event: events) {
            if (event.date == date) {
                events_by_date.push_back(event);
            }
        }
         return events_by_date;
    }

    vector<Reminder> getAllReminders() {
         return reminders;
    }
    vector<Event> getAllEvents() {
         return events;
    }
private:
    vector<Observer*> observers;
    vector<Reminder> reminders;
    vector<Event> events;
    void notify(const Reminder& reminder) {
        for (Observer* observer : observers) {
            observer->update(reminder);
        }
    }
};

// Абстрактный интерфейс стратегии
class ReminderStrategy {
public:
    virtual ~ReminderStrategy() = default;
    virtual void remind(const Reminder& reminder) = 0;
};

// Конкретные стратегии
class DefaultReminder : public ReminderStrategy {
public:
    void remind(const Reminder& reminder) override {
        cout << "Напоминание: " << reminder.message << " (" << reminder.date << ")" << endl;
    }
};
class PrioritizedReminder : public ReminderStrategy {
public:
    void remind(const Reminder& reminder) override {
        cout << "[Важно] Напоминание: " << reminder.message << " (" << reminder.date << ")" << endl;
    }
};

class ReminderScheduler {
public:
    ReminderScheduler(unique_ptr<ReminderStrategy> strategy) : reminderStrategy(move(strategy)){}
     void setReminderStrategy(unique_ptr<ReminderStrategy> strategy) {
        reminderStrategy = move(strategy);
    }

    void runReminders(const vector<Reminder>& reminders){
        for (const auto& reminder : reminders) {
            reminderStrategy->remind(reminder);
        }
    }
private:
    unique_ptr<ReminderStrategy> reminderStrategy;
};


int main() {
    // Создание календаря
    Calendar calendar;

    // Создание наблюдателей
    ConsoleObserver consoleObserver;
    EmailObserver emailObserver;
     
    // Добавление наблюдателей в календарь
    calendar.addObserver(&consoleObserver);
    calendar.addObserver(&emailObserver);

    // Создание событий
    Date eventDate(15, 11, 2024);
    Event event1(eventDate,"Встреча с друзьями");
    calendar.addEvent(event1);

    // Создание напоминаний
     Date reminderDate1(10, 11, 2024);
    Reminder reminder1(reminderDate1, "Купить подарки");
    calendar.addReminder(reminder1);

    Date reminderDate2(15, 11, 2024);
    Reminder reminder2(reminderDate2, "Поздравить с днем рождения");
    calendar.addReminder(reminder2);

    // Получение напоминаний и событий по дате
    Date dateForCheck(15, 11, 2024);
    cout << "События на " << dateForCheck << ":" << endl;
    for (const auto& event: calendar.getEventsByDate(dateForCheck)){
        cout << " - " << event.description << endl;
    }
    cout << "Напоминания на " << dateForCheck << ":" << endl;
    for (const auto& reminder: calendar.getRemindersByDate(dateForCheck)){
       cout << " - " << reminder.message << endl;
    }

    // Вывод всех напоминаний
    cout << "Все напоминания:" << endl;
    for (const auto& reminder: calendar.getAllReminders()){
        cout << " - " << reminder.message << " (" << reminder.date << ")" << endl;
    }

    // Создание планировщика напоминаний
    ReminderScheduler scheduler(make_unique<DefaultReminder>());

    // Запуск напоминаний
    scheduler.runReminders(calendar.getAllReminders());

     // Изменение стратегии
    scheduler.setReminderStrategy(make_unique<PrioritizedReminder>());
    scheduler.runReminders(calendar.getAllReminders());

    // Удаление наблюдателя
    calendar.removeObserver(&emailObserver);
    Date reminderDate3(16,11,2024);
     Reminder reminder3(reminderDate3, "Сходить в кино");
    calendar.addReminder(reminder3);


    return 0;
}
