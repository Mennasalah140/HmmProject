#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <unistd.h>



#define Metadata_Size 8

#define sbrk_Size 100000



struct free_node {

    int length;

    struct free_node *pre;

    struct free_node *next;

};



struct alloc_node {

    int metadata;

};

static struct free_node *myhead = NULL;



void merge()

{

    struct free_node *mynode = myhead;

    while (mynode != NULL && mynode->next != NULL) {

	if ((struct free_node *) ((char *) mynode + mynode->length) ==

	    mynode->next) {

	    mynode->length += mynode->next->length;

	    if (mynode->next->next != NULL) {

		mynode->next->next->pre = mynode;

		mynode->next = mynode->next->next;

	    } else {

		mynode->next = NULL;

	    }

	} else {

	    mynode = mynode->next;

	}

    }

    return;

}



struct free_node *first_fit(int size)

{

    struct free_node *mynode = myhead;

    while (mynode != NULL) {

	if (mynode->length == size) {

	    if (mynode->pre == NULL && mynode->next == NULL) {

		myhead = NULL;

	    } else if (mynode->pre == NULL) {

		myhead = mynode->next;

		mynode->next->pre = NULL;

	    } else if (mynode->next == NULL) {

		mynode->pre->next = NULL;

	    } else {

		mynode->pre->next = mynode->next;

		mynode->next->pre = mynode->pre;



	    }

	    return mynode;

	} else if (mynode->length > size) {



	    struct free_node *newnode =

		(struct free_node *) ((char *) mynode + size);

	    if (mynode->pre != NULL) {

		mynode->pre->next = newnode;

	    } else {

		myhead = newnode;

	    }

	    newnode->length = mynode->length - size;

	    return mynode;

	} else {

	    mynode = mynode->next;

	}

    }

    return mynode;

}



void *HmmAlloc(size_t size)

{

    int x = size;

    if (x < sizeof(struct free_node *)) {

        x += sizeof(struct free_node *);

        size += sizeof(struct free_node *);

    }

    size += Metadata_Size;

    int perfectsize = x % 8;	// To Make the address divisibe by 8

    if (perfectsize != 0) {

	perfectsize = 8 - perfectsize;

	size += perfectsize;

	x += perfectsize;

    }

    struct free_node *currentnode = first_fit(size);

    merge();

    while (currentnode == NULL) {

	struct free_node *new_node = sbrk(0);

	sbrk(sbrk_Size);

	new_node->length = sbrk_Size;

	new_node->next = NULL;

	struct free_node *temp = myhead;

	if (temp == NULL) {

	    myhead = new_node;

	    myhead->pre = NULL;

	    // myhead->next=NULL;

	} else {

	    while (temp->next != NULL) {

		temp = temp->next;

	    }

	    temp->next = new_node;

	    new_node->pre = temp;

	}

	merge();

	currentnode = first_fit(size);

	merge();

    }

    if (currentnode != NULL) {

	struct alloc_node *data = (struct alloc_node *) currentnode;

	data->metadata = x;

	// Take the address 16 bytes after the integer x

	void *ptr = (void *) ((char *) currentnode + Metadata_Size);

	return ptr;

    }

}



void Add_freelist(struct alloc_node *data, int size)

{

    size = size + Metadata_Size;

    struct free_node *newnode = (struct free_node *) data;

    struct free_node *mynode = myhead;



    if (mynode == NULL || newnode < mynode) {

	myhead = newnode;

	myhead->next = mynode;

	if (mynode != NULL) {

	    mynode->pre = myhead;

	}

	myhead->pre = NULL;

	myhead->length = size;

	return;

    }

    // Traverse the list to find the correct position to insert newnode

    while (mynode->next != NULL && newnode > mynode->next) {

	mynode = mynode->next;

    }

    // Insert newnode between current and current->next

    newnode->next = mynode->next;

    if (mynode->next != NULL) {

	mynode->next->pre = newnode;

    }

    newnode->pre = mynode;

    mynode->next = newnode;

    newnode->length = size;

}



void HmmFree(void *ptr)

{

    if (ptr == NULL) {

	return;

    }

    struct alloc_node *data =

	(struct alloc_node *) ((char *) ptr - Metadata_Size);

    int size = data->metadata;

    Add_freelist(data, size);

    merge();

    struct free_node *mynode = myhead;



    while (mynode->next != NULL) {

	mynode = mynode->next;

    }

    if (((char *) mynode + mynode->length) == sbrk(0)) {

	int x = (mynode->length) / sbrk_Size;

	mynode->length -= x * sbrk_Size;

	if (mynode->length == 0) {

	    if (mynode->pre != NULL) {

		mynode->pre->next = NULL;

	    } else {

		myhead = NULL;

	    }

	}

	sbrk(-sbrk_Size * x);

    }



}



void *HmmCalloc(size_t nmemb, size_t size)

{

    if (size <= 0 || nmemb <= 0) {

	return NULL;

    }

    void *ptr = HmmAlloc(nmemb * size);

    memset(ptr, 0, nmemb * size);

    return ptr;



}



void *HmmRealloc(void *ptr, size_t size)

{

    if (ptr == NULL) {

	void *newptr = HmmAlloc(size);

	return newptr;

    }



    if (size == 0 && ptr != NULL) {

	HmmFree(ptr);

	return NULL;

    }



    struct alloc_node *nodeptr =

	(struct alloc_node *) ((char *) ptr - Metadata_Size);

    int oldsize = nodeptr->metadata;

    if (oldsize >= size) {

	return ptr;

    }

    void *newptr = HmmAlloc(size);

    memcpy(newptr, ptr, oldsize);

    HmmFree(ptr);

    return newptr;



}





void *malloc(size_t size)

{

    void *ptr = HmmAlloc(size);

    return ptr;

}



void free(void *ptr)

{

    HmmFree(ptr);

    return;

}



void *calloc(size_t nmemb, size_t size)

{

    void *ptr = HmmCalloc(nmemb, size);

    return ptr;

}



void *realloc(void *ptr, size_t size)

{

    void *newptr = HmmRealloc(ptr, size);

    return newptr;

}





#if 0

int main(int argc, char *argv[])

{

    printf("current program break = %p \n", (char *) sbrk(0));

    int size = atoi(argv[1]);

    printf("current program break = %p \n", (char *) sbrk(0));

    void *ptr = HmmAlloc(size);

    printf("first : %p \n", ptr);

    printf("current program break = %p \n", (char *) sbrk(0));

    void *ptr1 = HmmAlloc(size);

    printf("second : %p \n", ptr1);

    void *ptr2 = HmmAlloc(size);

    printf("third : %p \n", ptr2);

    // printf("after program break = %p \n", (char *) sbrk(0));

    // struct alloc_node *data =

    (struct alloc_node *) ((char *) ptr - Metadata_Size);



    // printf("%d \n", data->metadata);

    // printf("%p \n", ptr1);

    // printf("%p \n", ptr2);

    // HmmFree(ptr1);

    // HmmFree(ptr2);

    //  void *ptr3 = HmmRealloc(ptr,60);

    // printf("realloc : %p \n",ptr3);

    struct free_node *mynode = myhead;



    while (mynode != NULL) {

	printf("length= %d , address = %p \n", mynode->length, mynode);

	mynode = mynode->next;

    }

    HmmFree(0x0);

    // printf("after free = %p \n", (char *) sbrk(0));





    return 0;



}



#endif
