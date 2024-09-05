import java.util.*;

class Train {
    int number;
    String name;
    String origin;
    String destination;
    int availableSeats;
    int fare;
    Queue<String> waitingList;

    public Train(int number, String name, String origin, String destination, int availableSeats, int fare) {
        this.number = number;
        this.name = name;
        this.origin = origin;
        this.destination = destination;
        this.availableSeats = availableSeats;
        this.fare = fare;
        this.waitingList = new LinkedList<>();
    }
}

class Booking {
    int bookingId;
    Train train;
    String passengerName;

    public Booking(int bookingId, Train train, String passengerName) {
        this.bookingId = bookingId;
        this.train = train;
        this.passengerName = passengerName;
    }
}

public class TrainBookingSystem {

    static ArrayList<Train> trains = new ArrayList<>();
    static ArrayList<Booking> bookings = new ArrayList<>();
    static int bookingIdCounter = 1;

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        populateTrains();

        while (true) {
            System.out.println("1. Book Ticket");
            System.out.println("2. View Booking");
            System.out.println("3. Cancel Booking");
            System.out.println("4. Exit");
            System.out.print("Enter your choice: ");
            int choice = scanner.nextInt();

            switch (choice) {
                case 1:
                    bookTicket(scanner);
                    break;
                case 2:
                    viewBooking(scanner);
                    break;
                case 3:
                    cancelBooking(scanner);
                    break;
                case 4:
                    System.out.println("Thank you for using the Train Booking System.");
                    return;
                default:
                    System.out.println("Invalid choice. Please try again.");
            }
        }
    }

    public static void populateTrains() {
        trains.add(new Train(1234, "Rajdhani Express", "New Delhi", "Mumbai", 2, 1500));
        trains.add(new Train(5678, "Shatabdi Express", "Chennai", "Bangalore", 1, 1200));
        trains.add(new Train(9101, "Duronto Express", "Kolkata", "Delhi", 1, 2000));
        trains.add(new Train(1122, "Garib Rath", "Mumbai", "Jaipur", 1, 800));
    }

    public static void bookTicket(Scanner scanner) {
        System.out.print("Enter passenger name: ");
        String passengerName = scanner.next();

        System.out.println("Available trains:");
        for (int i = 0; i < trains.size(); i++) {
            Train train = trains.get(i);
            System.out.println((i + 1) + ". " + train.name + " (" + train.origin + " to " + train.destination + "), Available seats: " + train.availableSeats + ", Fare: " + train.fare);
        }

        System.out.print("Select train by number: ");
        int trainChoice = scanner.nextInt();
        if (trainChoice < 1 || trainChoice > trains.size()) {
            System.out.println("Invalid train selection.");
            return;
        }

        Train selectedTrain = trains.get(trainChoice - 1);

        if (selectedTrain.availableSeats > 0) {
            selectedTrain.availableSeats--;
            Booking booking = new Booking(bookingIdCounter++, selectedTrain, passengerName);
            bookings.add(booking);
            System.out.println("Booking successful! Your booking ID is " + booking.bookingId);
        } else {
            System.out.println("No seats available for this train.");
            selectedTrain.waitingList.add(passengerName);
            System.out.println("You have been added to the waiting list. Position in waiting list: " + selectedTrain.waitingList.size());
        }
    }

    public static void viewBooking(Scanner scanner) {
        System.out.print("Enter booking ID: ");
        int bookingId = scanner.nextInt();

        Booking foundBooking = null;
        for (Booking booking : bookings) {
            if (booking.bookingId == bookingId) {
                foundBooking = booking;
                break;
            }
        }

        if (foundBooking != null) {
            System.out.println("Booking ID: " + foundBooking.bookingId);
            System.out.println("Passenger Name: " + foundBooking.passengerName);
            System.out.println("Train: " + foundBooking.train.name);
            System.out.println("From: " + foundBooking.train.origin + " To: " + foundBooking.train.destination);
        } else {
            System.out.println("Booking not found.");
        }
    }

    public static void cancelBooking(Scanner scanner) {
        System.out.print("Enter booking ID to cancel: ");
        int bookingId = scanner.nextInt();

        Booking foundBooking = null;
        for (Booking booking : bookings) {
            if (booking.bookingId == bookingId) {
                foundBooking = booking;
                break;
            }
        }

        if (foundBooking != null) {
            foundBooking.train.availableSeats++;
            bookings.remove(foundBooking);
            System.out.println("Booking canceled successfully.");

            // Check waiting list
            if (!foundBooking.train.waitingList.isEmpty()) {
                String waitingPassenger = foundBooking.train.waitingList.poll();
                foundBooking.train.availableSeats--;
                Booking newBooking = new Booking(bookingIdCounter++, foundBooking.train, waitingPassenger);
                bookings.add(newBooking);
                System.out.println("Seat assigned to passenger in waiting list: " + waitingPassenger + ". New booking ID: " + newBooking.bookingId);
            }

        } else {
            System.out.println("Booking not found.");
        }
    }
}
