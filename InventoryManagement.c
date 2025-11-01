#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct product
{
    int ID;
    char name[50];
    float price;
    int quantity;
}product;

enum MenuOption
{
    ADD_PRODUCT = 1,
    VIEW_PRODUCTS,
    UPDATE_QUANTITY,
    SEARCH_BY_ID,
    SEARCH_BY_NAME,
    SEARCH_BY_PRICE_RANGE,
    DELETE_PRODUCT,
    EXIT_PROGRAM
};

product *inventory = NULL;
int totalProductCount = 0;

void readProduct(product *item)
{
    printf("Proct ID: ");
    scanf("%d",&item->ID);
    getchar();
    printf("Product Name: ");
    fgets(item->name, sizeof(item->name), stdin);
    item->name[strcspn(item->name, "\n")] = '\0';
    printf("Price: ");
    scanf("%f",&item->price);
    printf("Quantity: ");
    scanf("%d",&item->quantity);
}

void show_menu() {
    printf("\n========= INVENTORY MENU =========\n");
    printf("1. Add New Product\n");
    printf("2. View All Products\n");
    printf("3. Update Quantity\n");
    printf("4. Search Product by ID\n");
    printf("5. Search Product by Name\n");
    printf("6. Search Product by Price Range\n");
    printf("7. Delete Product\n");
    printf("8. Exit\n");
}

void addProduct()
{
    product *temp = realloc(inventory, (totalProductCount + 1) * sizeof(product));
    if(temp == NULL)
    {
        printf("Allocation failed\n");
        return;
    }

    inventory = temp;

    printf("\nEnter details for new product:\n");
    readProduct(&inventory[totalProductCount]);
    totalProductCount++;
    printf("Product added successfully.\n");
}

void viewProduct()
{
    if (totalProductCount == 0) {
        printf("No products available.\n");
        return;
    }
    
    printf("\n========= PRODUCT LIST =========\n");

   for (int i = 0; i < totalProductCount; i++) {
        printf("ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
                   inventory[i].ID,
                   inventory[i].name,
                   inventory[i].price,
                   inventory[i].quantity);
    }
}

void updateQuantity()
{
    int targetID, newQuantity;
    bool found = false;
    printf("Enter Product ID to update quantity: ");
    scanf("%d",&targetID);

    for (int i = 0; i<totalProductCount; i++)
    {
        if(inventory[i].ID == targetID)
        {
            printf("Enter new quantity: ");
            scanf("%d",&newQuantity);
            inventory[i].quantity = newQuantity;
            found = true;
            printf("Quantity updated successfully.\n");
            break;
        }
    }
    if(found == false)
    {
        printf("Product with ID %d not found.\n", targetID);
    }
}

void searchByID() {
    int target_id;
    bool found = false;
    printf("Enter Product ID to search: ");
    scanf("%d", &target_id);
    for (int i = 0; i < totalProductCount; i++) 
    {
        if (inventory[i].ID == target_id) 
        {
            printf("ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
                   inventory[i].ID,
                   inventory[i].name,
                   inventory[i].price,
                   inventory[i].quantity);
            found = true;
            break;
        }
    }
    if (found == false) {
        printf("Product with ID %d not found.\n", target_id);
    }
}

void search_by_name() {
    char keyword[50];
    bool found = false;
    getchar(); 
    printf("Enter name to search (partial names allowed): ");
    fgets(keyword, sizeof(keyword), stdin);
    keyword[strcspn(keyword, "\n")] = '\0';
    printf("Products Found:\n");
    for (int i = 0; i < totalProductCount; i++)
    {
        if (strstr(inventory[i].name, keyword) != NULL)
        {
            printf("ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
                   inventory[i].ID,
                   inventory[i].name,
                   inventory[i].price,
                   inventory[i].quantity);
            found = true;
        }
    }
    if (found == false) 
    {
        printf("No products found matching '%s'.\n", keyword);
    }
}

void search_by_price() {
    float min_price, max_price;
    bool found = false;
    printf("Enter minimum price: ");
    scanf("%f", &min_price);
    printf("Enter maximum price: ");
    scanf("%f", &max_price);
    printf("Products in price range:\n");

    for (int i = 0; i < totalProductCount; i++) 
    {
        if (inventory[i].price >= min_price && inventory[i].price <= max_price) 
        {
            printf("ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
                   inventory[i].ID,
                   inventory[i].name,
                   inventory[i].price,
                   inventory[i].quantity);
            found = true;
        }
    }
    if (found == false) 
    {
        printf("No products found in the given price range.\n");
    }
}

void deleteProduct()
{
    if (totalProductCount == 0)
    {
        printf("No products available to delete.\n");
        return;
    }

    int targetID;
    bool found = false;
    printf("Enter Product ID to delete: ");
    scanf("%d", &targetID);

    for (int i = 0; i < totalProductCount; i++)
    {
        if (inventory[i].ID == targetID)
        {
            found = true;
            for (int j = i; j < totalProductCount - 1; j++)
            {
                inventory[j] = inventory[j + 1];
            }

            totalProductCount--;

            product *temp = realloc(inventory, totalProductCount * sizeof(product));

            if (temp != NULL || totalProductCount == 0) 
            {
                inventory = temp;
            }

            printf("Product with ID %d deleted successfully.\n", targetID);
            break;
        }
    }

    if (found == false)
    {
        printf("Product with ID %d not found.\n", targetID);
    }
}

int main()
{
    int choice;

    printf("Enter the number of initial products: ");
    scanf("%d", &totalProductCount);
    getchar();

    if (totalProductCount > 0)
    {
        inventory = calloc(totalProductCount, sizeof(product));
        if (inventory == NULL)
        {
            printf("Memory allocation failed.\n");
            return 1;
        }

        for (int i = 0; i < totalProductCount; i++)
        {
            printf("\nEnter details for product %d:\n", i + 1);
            readProduct(&inventory[i]);
        }
    }

    do
    {
        show_menu();
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case ADD_PRODUCT:
            addProduct();
            break;
        case VIEW_PRODUCTS:
            viewProduct();
            break;
        case UPDATE_QUANTITY:
            updateQuantity();
            break;
        case SEARCH_BY_ID:
            searchByID();
            break;
        case SEARCH_BY_NAME:
            search_by_name();
            break;
        case SEARCH_BY_PRICE_RANGE:
            search_by_price();
            break;
        case DELETE_PRODUCT:
            deleteProduct();
            break;
        case EXIT_PROGRAM:
            printf("Releasing allocated memory and exiting program.\n");
            break;
        default:
            printf("Invalid choice. Enter a number between 1 and 8.\n");
        }

    } while (choice != EXIT_PROGRAM);

    free(inventory);
    inventory = NULL;
    return 0;
}
