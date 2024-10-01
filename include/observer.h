#ifndef OBSERVER_H
#define OBSERVER_H

#include <stdlib.h>

// Error codes
#define OBSERVER_SUCCESS 0
#define OBSERVER_ERROR_NULL_PTR -1
#define OBSERVER_ERROR_MAX_OBSERVERS -2

// Forward declaration of Subject and Observer
typedef struct Observer Observer;
typedef struct Subject Subject;

// Observer structure
struct Observer {
    void (*update)(Subject*, void* observerData);  // Function pointer to update observer
    void* observerData;  // Data specific to the observer
};

// Subject structure
struct Subject {
    void* subjectData;  // Data associated with the subject
    Observer** observers;  // Array of pointers to observers
    int observerCount;
    int maxObservers;
};

// Function prototypes for the observer pattern
Subject create_subject(void* subjectData, int maxObservers);
int add_observer(Subject* subject, Observer* observer);
int remove_observer(Subject* subject, Observer* observer);
int notify_observers(Subject* subject);
int cleanup_subject(Subject* subject);

// Implementation section
#ifdef OBSERVER_IMPLEMENTATION

// Create a new subject
Subject create_subject(void* subjectData, int maxObservers) {
    Subject subject;
    subject.subjectData = subjectData;
    subject.maxObservers = maxObservers;
    subject.observerCount = 0;
    subject.observers = (Observer**) malloc(sizeof(Observer*) * maxObservers);
    return subject;
}

// Add an observer to the subject
int add_observer(Subject* subject, Observer* observer) {
    if (subject == NULL || observer == NULL) {
        return OBSERVER_ERROR_NULL_PTR;
    }
    if (subject->observerCount >= subject->maxObservers) {
        return OBSERVER_ERROR_MAX_OBSERVERS;
    }
    subject->observers[subject->observerCount++] = observer;
    return OBSERVER_SUCCESS;
}

// Remove an observer from the subject
int remove_observer(Subject* subject, Observer* observer) {
    if (subject == NULL || observer == NULL) {
        return OBSERVER_ERROR_NULL_PTR;
    }
    for (int i = 0; i < subject->observerCount; ++i) {
        if (subject->observers[i] == observer) {
            for (int j = i; j < subject->observerCount - 1; ++j) {
                subject->observers[j] = subject->observers[j + 1];
            }
            subject->observerCount--;
            return OBSERVER_SUCCESS;
        }
    }
    return OBSERVER_SUCCESS;
}

// Notify all observers
int notify_observers(Subject* subject) {
    if (subject == NULL) {
        return OBSERVER_ERROR_NULL_PTR;
    }
    for (int i = 0; i < subject->observerCount; ++i) {
        Observer* observer = subject->observers[i];
        observer->update(subject, observer->observerData);
    }
    return OBSERVER_SUCCESS;
}

// Clean up the subject
int cleanup_subject(Subject* subject) {
    if (subject == NULL) {
        return OBSERVER_ERROR_NULL_PTR;
    }
    free(subject->observers);
    subject->observers = NULL;
    return OBSERVER_SUCCESS;
}

#endif // OBSERVER_IMPLEMENTATION

#endif // OBSERVER_H
