#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Define structures for train, passenger, booking, and nodes

struct train {
    int train_number;
    char source[50];
    char destination[50];
    int no_of_seats;
    char time[50];
};

struct passenger {
    char name[50];
    int age;
    char gender;
};

struct booking {
    int pnr;
    struct train train_info;
    struct passenger passenger_info;
};

struct queue_node {
    struct booking booking_info;
    struct queue_node* next;
};

struct train_node {
    struct train train_info;
    struct queue_node* bookings;
    struct train_node* left;
    struct train_node* right;
    struct queue_node* waitinglist;
};

struct route_node {
    struct train_node* train_schedule;
    struct route_node* next;
};

int admin_login();
int admin_passwd_valid(char uname[10], char pwd[12]);
struct route_node* admin_menu(struct route_node* head);
void user_menu(struct route_node* head);
struct booking* dequeue(struct queue_node** front_ref, struct queue_node** rear_ref);
void enqueue(struct queue_node** front_ref, struct queue_node** rear_ref, struct booking booking_info);
struct train_node* find_train_containing_booking(struct route_node* head, int pnr);
struct train_node* create_train_node(int train_number, char *source, char *destination,int seats,char *time);
void add_booking_to_train(struct train_node* train, struct booking booking_info);
struct train_node* create_train_schedule();
struct route_node* create_route_node(struct train_node* train_schedule);
void add_route(struct route_node** head, struct train_node* train_schedule);
void display_schedule(struct train_node* root);
void display_routes(struct route_node* head);
void book_ticket(struct route_node* head, struct passenger passenger_info, int train_number, int pnr);
void cancel_ticket(struct route_node* head, int pnr);
int check_seat_availability(struct route_node* head, int train_number);
void print_ticket(struct route_node* head, int pnr);

// Function to create a new train node with an empty bookings list
struct train_node* create_train_node(int train_number, char *source, char *destination,int seats,char *time) {
    struct train_node* new_node = (struct train_node*)malloc(sizeof(struct train_node));
    new_node->train_info.train_number = train_number;
    new_node->train_info.no_of_seats = seats;
    strcpy(new_node->train_info.source, source);
    strcpy(new_node->train_info.destination, destination);
    strcpy(new_node->train_info.time, time);
    new_node->bookings = NULL; // Initialize bookings list
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->waitinglist=NULL;
    return new_node;
}

// Function to create a new route node
struct route_node* create_route_node(struct train_node* train_schedule) {
    struct route_node* new_node = (struct route_node*)malloc(sizeof(struct route_node));
    new_node->train_schedule = train_schedule;
    new_node->next = NULL;
    return new_node;
}

// Function to add a booking to a train's bookings list
void add_booking_to_train(struct train_node* train, struct booking booking_info) {
    struct queue_node* new_booking = (struct queue_node*)malloc(sizeof(struct queue_node));
    new_booking->booking_info = booking_info;
    new_booking->next = train->bookings;
    train->bookings = new_booking;
}

// Function to enqueue a booking into the ticket queue
void enqueue(struct queue_node** front_ref, struct queue_node** rear_ref, struct booking booking_info) {
    struct queue_node* new_node = (struct queue_node*)malloc(sizeof(struct queue_node));
    new_node->booking_info = booking_info;
    new_node->next = NULL;
    if (*rear_ref == NULL) {
        *front_ref = new_node;
    } else {
        (*rear_ref)->next = new_node;
    }
    *rear_ref = new_node;
}

// Function to dequeue a booking from the ticket queue
struct booking* dequeue(struct queue_node** front_ref, struct queue_node** rear_ref) {
    if (*front_ref == NULL) {
        printf("Queue Underflow\n");
        return NULL;
    }
    struct queue_node* temp = *front_ref;
    struct booking* dequeued = (struct booking*)malloc(sizeof(struct booking)); // Allocate memory
    *dequeued = temp->booking_info;
    *front_ref = temp->next;
    if (*front_ref == NULL) {
        *rear_ref = NULL;
    }
    free(temp);
    return dequeued;
}

// Function to display train schedule
void display_schedule(struct train_node* root) {
    if (root != NULL) {
        printf("Train Number: %d\t, Source: %s\t, Destination: %s\t Time: %s\n", root->train_info.train_number, root->train_info.source, root->train_info.destination ,root->train_info.time);
        display_schedule(root->left);
        display_schedule(root->right);
    }
}

// Function to display train routes
void display_routes(struct route_node* head) {
    struct route_node* current = head;
    while (current != NULL) {
        display_schedule(current->train_schedule);
        current = current->next;
    }
}
struct queue_node* find_rear_of_waiting_list(struct queue_node* front) {
    if (front == NULL) {
        return NULL; // If the queue is empty, return NULL
    }

    struct queue_node* current = front;

    // Traverse until the last node
    while (current->next != NULL) {
        current = current->next;
    }

    return current; // Return the last node, which is the rear of the waiting list
}


void book_ticket(struct route_node* head, struct passenger passenger_info, int train_number, int pnr) {
    struct route_node* current = head;
    int found_train = 0; // Flag to track if the train is found
    struct queue_node* p;


    // Traverse the route nodes
    while (current != NULL) {
        struct train_node* train = current->train_schedule;
         struct queue_node* rear_of_waiting_list = find_rear_of_waiting_list(train->waitinglist);

        // Find the train with the specified train number
        while (train != NULL) {
            if (train->train_info.train_number == train_number) {
                found_train = 1; // Set flag to indicate train is found
                // Loop through the bookings and count the number of booked seats
                struct queue_node* booking = train->bookings;
                int booked_seats = 0;
                while (booking != NULL) {
                    booked_seats++;
                    // Increment the count of booked seats
                    booking = booking->next; // Move to the next booking
                }

                // Check if there are available seats
                if (booked_seats < train->train_info.no_of_seats) { // Assuming each train has 2 seats
                    // Book the ticket
                    struct booking new_booking;
                    new_booking.pnr = pnr;
                    new_booking.passenger_info = passenger_info;
                    new_booking.train_info = train->train_info;
                    add_booking_to_train(train, new_booking); // Add booking to train's bookings list
                    printf("Ticket booked successfully! PNR: %d\n", pnr);
                    return;
                } else {
                    // If all seats are booked, enqueue the passenger onto the waiting list
                    struct booking new_booking;
                    new_booking.pnr = pnr;
                    new_booking.passenger_info = passenger_info;
                    new_booking.train_info = train->train_info;
                    enqueue(&(train->waitinglist), &rear_of_waiting_list, new_booking);
                    printf("You are added to the waiting list for train %d.\n", train_number);
                    return;
                }
            }
            train = train->right; // Move to the next train
        }
        current = current->next; // Move to the next route node
    }

    // If the train with the specified number is not found
    if (!found_train) {
        printf("Train with number %d not found!\n", train_number);
    }
}



// Function to cancel a ticket based on PNR from a train's bookings list
struct booking* cancel_ticket_from_train(struct train_node* train, int pnr) {
    struct queue_node* current = train->bookings;
    struct queue_node* prev = NULL;
    struct queue_node* rear_of_waiting_list = find_rear_of_waiting_list(train->waitinglist);
    while (current != NULL) {
        if (current->booking_info.pnr == pnr) {
            if (prev == NULL) {
                // If the booking to be canceled is the first node
                train->bookings = current->next;
            } else {
                prev->next = current->next;
            }
            printf("Ticket with PNR %d canceled successfully!\n", pnr);
            struct queue_node* book = train->bookings;
            int count = 0;

            while (book != NULL) {
                count++;
                book = book->next;
            }
            while (count < train->train_info.no_of_seats) {
                if (train->waitinglist != NULL) {
                    struct booking* confirm = dequeue(&(train->waitinglist), &rear_of_waiting_list);
                    add_booking_to_train(train, *confirm);
                    printf("Booking for PNR %d is confirmed\n", confirm->pnr);
                }
                count++;
            }
            return &(current->booking_info); // Return the canceled booking
        }
        prev = current;
        current = current->next;
    }
    printf("Ticket with PNR %d not found!\n", pnr);
    return NULL; // Return NULL if the ticket is not found
}


// Function to check seat availability for a specific train
int check_seat_availability(struct route_node* head, int train_number) {
    struct route_node* current = head;
    struct queue_node* c;
    int count=0;
    while (current != NULL) {
        struct train_node* train = current->train_schedule;
        while (train != NULL) {
            if (train->train_info.train_number == train_number) {
                // For simplicity, let's assume each train has 100 seats and check if any ticket is booked
                c=train->bookings;
                while(c!=NULL){
                    count=count+1;
                     c = c->next;
                }
                count=(train->train_info.no_of_seats)-count;
                return count;
            }
            train = train->right;
        }
        current = current->next;
    }
    // If the train number is not found, return -1
    return -1;
}

// Function to cancel a ticket based on PNR
void cancel_ticket(struct route_node* head, int pnr) {
    struct route_node* current = head;
    while (current != NULL) {
        struct train_node* train = current->train_schedule;
        while (train != NULL) {
            cancel_ticket_from_train(train, pnr);
            train = train->right;
        }
        current = current->next;
    }
}

// Function to find the train containing the booking with the specified PNR
struct train_node* find_train_containing_booking(struct route_node* head, int pnr) {
    struct route_node* current = head;
    while (current != NULL) {
        struct train_node* train = current->train_schedule;
        while (train != NULL) {
            struct queue_node* booking = train->bookings;
            while (booking != NULL) {
                if (booking->booking_info.pnr == pnr) {
                    return train;
                }
                booking = booking->next;
            }
            train = train->right;
        }
        current = current->next;
    }
    return NULL; // Booking with specified PNR not found
}
// Function to create a new train schedule
struct train_node* create_train_schedule() {
    int train_number, num_stations,seats;
    char source[50], destination[50],time[50];

    printf("Enter the number of trains: ");
    scanf("%d", &num_stations);

    struct train_node* root = NULL;

    for (int i = 0; i < num_stations; i++) {
        printf("\nEnter details for train %d:\n", i + 1);
        printf("Train Number: ");

        scanf("%d", &train_number);
        printf("\nEnter no of seats:\n");
         scanf("%d", &seats);
        printf("Source: ");
        scanf(" %[^\n]", source);
        printf("Destination: ");
        scanf(" %[^\n]", destination);
        printf("time of departure: ");
        scanf(" %[^\n]", time);

        struct train_node* new_node = create_train_node(train_number, source, destination,seats,time);

        if (root == NULL) {
            root = new_node;
        } else {
            struct train_node* current = root;
            while (1) {
                if (new_node->train_info.train_number < current->train_info.train_number) {
                    if (current->left == NULL) {
                        current->left = new_node;
                        break;
                    }
                    current = current->left;
                } else {
                    if (current->right == NULL) {
                        current->right = new_node;
                        break;
                    }
                    current = current->right;
                }
            }
        }
    }

    return root;
}

// Function to display train ticket based on PNR
void print_ticket(struct route_node *head, int pnr) {
    struct route_node *current = head;

    // Stack for iterative traversal
    struct route_node *stack[100];
    int top = -1;

    while (current != NULL || top != -1) {
        // Traverse left subtree and push nodes onto the stack
        while (current != NULL) {
            stack[++top] = current;
            current = current->next;
        }

        // Pop a node from the stack
        current = stack[top--];

        // Search bookings in the current train node
        struct queue_node *booking = current->train_schedule->bookings;
        while (booking != NULL) {
            if (booking->booking_info.pnr == pnr) {
                printf("Ticket Details:\n");
                printf("PNR: %d\n", booking->booking_info.pnr);
                printf("Passenger Name: %s\n", booking->booking_info.passenger_info.name);
                printf("Age: %d\n", booking->booking_info.passenger_info.age);
                printf("Gender: %c\n", booking->booking_info.passenger_info.gender);
                printf("Train Number: %d\n", booking->booking_info.train_info.train_number);
                printf("Source: %s\n", booking->booking_info.train_info.source);
                printf("Destination: %s\n", booking->booking_info.train_info.destination);
                printf("Time: %s\n", booking->booking_info.train_info.time);
                return; // Exit the function after printing the ticket details
            }
            booking = booking->next;
        }

        // Traverse right subtree
        current = current->next;
    }

    // If the ticket is not found
    printf("Ticket with PNR %d not found!\n", pnr);
}



// Function to generate a unique PNR
int generate_pnr() {
    // Get current system time
    time_t current_time = time(NULL);

    // Convert current time to string
    char* time_str = ctime(&current_time);

    // Generate a random number
    int random_num = rand();

    // Combine time and random number to form a unique PNR
    int pnr = atoi(time_str) + random_num;

    return pnr;
}

int admin_login() {
    char us_name[10];
    char pwd[12];
    int res;

    printf("Admin Login\n");
    printf("Enter username: ");
    scanf("%s", us_name);
    printf("Enter password: ");
    scanf("%s", pwd);

    res = admin_passwd_valid(us_name, pwd);
    return res;
}

int admin_passwd_valid(char uname[10], char pwd[12]) {
    // Dummy validation for demonstration
    if (strcmp(uname, "admin") == 0 && strcmp(pwd, "password") == 0) {
        return 1; // Valid credentials
    } else {
        return 0; // Invalid credentials
    }
}

// Admin menu function

struct route_node* admin_menu(struct route_node* head) {
    // Create train schedule
    struct route_node* temp_head = create_route_node(create_train_schedule());
    
    printf("Exiting Admin Menu...\n");
    return temp_head; 
}

// User menu function

void user_menu(struct route_node* head) {
    int choice, train_number, pnr , res;
    struct passenger passenger_info;
    srand(time(NULL));
    while(1){
        printf("\nTrain Ticket Booking System\n");
        printf("1. Display Train Schedule\n");
        printf("2. Display Train Routes\n");
        printf("3. Book Ticket\n");
        printf("4. Cancel Ticket\n");
        printf("5. Check Seat Availability\n");
        printf("6. Print Ticket\n");
        printf("7. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Train Schedule:\n");
                display_schedule(head->train_schedule);
                break;
            case 2:
                printf("Train Routes:\n");
                display_routes(head);
                break;
            case 3:
                // Book ticket
                printf("Enter passenger name: ");
                scanf(" %[^\n]", passenger_info.name);
                printf("Enter passenger age: ");
                scanf("%d", &passenger_info.age);
                printf("Enter passenger gender (M/F): ");
                scanf(" %c", &passenger_info.gender);
                printf("Enter train number: ");
                scanf("%d", &train_number);
                pnr = generate_pnr();
                book_ticket(head, passenger_info, train_number, pnr);
                break;
            case 4:
                // Cancel ticket
                printf("Enter PNR: ");
                scanf("%d", &pnr);
                cancel_ticket(head, pnr);
                break;
            case 5:
                // Check seat availability
                printf("Enter train number: ");
                scanf("%d", &train_number);
                int available_seats = check_seat_availability(head, train_number);
                if (available_seats != -1) {
                    printf("Available seats for train %d: %d\n", train_number, available_seats);
                } else {
                    printf("Train with number %d not found!\n", train_number);
                }
                break;
            case 6:
                //print ticket
                printf("Enter PNR: ");
                scanf("%d", &pnr);
                print_ticket(head, pnr);
                break;
            case 7:
                res = admin_login(); // Return to login screen for exit
                if(res != 1) {
                    printf("Invalid Login. Check the credentials. Please Try again.\n");
                    break;
                }
                else 
                {
                    printf("Admin verified. Access granted.\n");
                    return;
                }
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    } 
}

// Main function

int main() {
    int res = admin_login();// Start with admin login
    while(res != 1)
    {
        printf("Invalid Login. Check the credentials. Please Try again.\n");
        res = admin_login();
    }
    printf("Admin verified. Access granted.\n");    

    struct route_node* head = NULL;
    head = admin_menu(head);
    user_menu(head);
    return 0;
}
