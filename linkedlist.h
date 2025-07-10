#include <stdio.h>
#include <stdlib.h> // For malloc and free

// Define the structure for a node in the linked list
struct Node {
    int data;          // Data stored in the node
    struct Node* next; // Pointer to the next node in the list
};

// Function to create a new node
struct Node* createNode(int data) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    if (newNode == NULL) {
        printf("Memory allocation failed!\n");
        exit(1); // Exit if memory allocation fails
    }
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

// Function to insert a node at the beginning of the list
void insertAtBeginning(struct Node** headRef, int data) {
    struct Node* newNode = createNode(data);
    newNode->next = *headRef;
    *headRef = newNode;
}

// Function to insert a node at the end of the list
void insertAtEnd(struct Node** headRef, int data) {
    struct Node* newNode = createNode(data);
    if (*headRef == NULL) {
        *headRef = newNode;
        return;
    }
    struct Node* temp = *headRef;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newNode;
}

// Function to print the linked list
void printList(struct Node* head) {
    struct Node* temp = head;
    while (temp != NULL) {
        printf("%d -> ", temp->data);
        temp = temp->next;
    }
    printf("NULL\n");
}

// Function to delete a node from the beginning of the list
void deleteFromBeginning(struct Node** headRef) {
    if (*headRef == NULL) {
        printf("List is empty. Cannot delete.\n");
        return;
    }
    struct Node* temp = *headRef;
    *headRef = (*headRef)->next;
    free(temp); // Free the memory of the deleted node
}

// Function to delete a node from the end of the list
void deleteFromEnd(struct Node** headRef) {
    if (*headRef == NULL) {
        printf("List is empty. Cannot delete.\n");
        return;
    }
    if ((*headRef)->next == NULL) { // Only one node in the list
        free(*headRef);
        *headRef = NULL;
        return;
    }
    struct Node* temp = *headRef;
    while (temp->next->next != NULL) {
        temp = temp->next;
    }
    free(temp->next); // Free the memory of the last node
    temp->next = NULL;
}

