#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROWS 25
#define COLS 60
#define MAX_OBJECTS 50

#define EMPTY '_'
#define DRAW '*'

typedef enum {
    LINE = 1,
    RECTANGLE,
    CIRCLE,
    TRIANGLE
} ObjectType;

typedef struct {
    int id;
    ObjectType type;
    int x1, y1;
    int x2, y2;
    int x3, y3;
    int radius;
    int active;
} Object;

char canvas[ROWS][COLS];
Object objects[MAX_OBJECTS];
int objectCount = 0;
int nextId = 1;

void clearInputBuffer(void) {
    int ch;

    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

int readInt(const char prompt[], int *value) {
    char line[100];
    char *start;
    char *end;
    long parsedValue;

    while (1) {
        printf("%s", prompt);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\nInput ended.\n");
            return 0;
        }

        if (strchr(line, '\n') == NULL) {
            printf("Please enter one valid number only.\n");
            clearInputBuffer();
            continue;
        }

        start = line;
        while (isspace((unsigned char)*start)) {
            start++;
        }

        if (*start == '\0') {
            printf("Please enter a valid number.\n");
            continue;
        }

        errno = 0;
        parsedValue = strtol(start, &end, 10);

        while (isspace((unsigned char)*end)) {
            end++;
        }

        if (errno == ERANGE || parsedValue < INT_MIN ||
            parsedValue > INT_MAX || *end != '\0') {
            printf("Please enter one valid number only.\n");
            continue;
        }

        *value = (int)parsedValue;
        return 1;
    }
}

int readBoundedInt(const char prompt[], int min, int max, int *value) {
    while (1) {
        if (!readInt(prompt, value)) {
            return 0;
        }

        if (*value >= min && *value <= max) {
            return 1;
        }

        printf("Value must be between %d and %d.\n", min, max);
    }
}

int readCanvasX(const char prompt[], int *x) {
    return readBoundedInt(prompt, 0, COLS - 1, x);
}

int readCanvasY(const char prompt[], int *y) {
    return readBoundedInt(prompt, 0, ROWS - 1, y);
}

int minInt(int a, int b) {
    return (a < b) ? a : b;
}

void swapInt(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void clearCanvas(void) {
    int row, col;

    for (row = 0; row < ROWS; row++) {
        for (col = 0; col < COLS; col++) {
            canvas[row][col] = EMPTY;
        }
    }
}

void plotPoint(int x, int y) {
    if (x >= 0 && x < COLS && y >= 0 && y < ROWS) {
        canvas[y][x] = DRAW;
    }
}

void displayPicture(void) {
    int row, col;

    printf("\n");
    printf("Canvas size: x = 0 to %d, y = 0 to %d\n\n", COLS - 1, ROWS - 1);

    for (row = 0; row < ROWS; row++) {
        for (col = 0; col < COLS; col++) {
            printf("%c", canvas[row][col]);
        }
        printf("\n");
    }

    printf("\n");
}

void drawLine(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    int e2;

    while (1) {
        plotPoint(x1, y1);

        if (x1 == x2 && y1 == y2) {
            break;
        }

        e2 = 2 * err;

        if (e2 > -dy) {
            err = err - dy;
            x1 = x1 + sx;
        }

        if (e2 < dx) {
            err = err + dx;
            y1 = y1 + sy;
        }
    }
}

void drawRectangle(int x1, int y1, int x2, int y2) {
    if (x1 > x2) {
        swapInt(&x1, &x2);
    }

    if (y1 > y2) {
        swapInt(&y1, &y2);
    }

    drawLine(x1, y1, x2, y1);
    drawLine(x2, y1, x2, y2);
    drawLine(x2, y2, x1, y2);
    drawLine(x1, y2, x1, y1);
}

void plotCirclePoints(int cx, int cy, int x, int y) {
    plotPoint(cx + x, cy + y);
    plotPoint(cx - x, cy + y);
    plotPoint(cx + x, cy - y);
    plotPoint(cx - x, cy - y);
    plotPoint(cx + y, cy + x);
    plotPoint(cx - y, cy + x);
    plotPoint(cx + y, cy - x);
    plotPoint(cx - y, cy - x);
}

void drawCircle(int cx, int cy, int radius) {
    int x = 0;
    int y = radius;
    int decision = 1 - radius;

    if (radius < 0) {
        return;
    }

    while (x <= y) {
        plotCirclePoints(cx, cy, x, y);
        x++;

        if (decision < 0) {
            decision = decision + (2 * x) + 1;
        } else {
            y--;
            decision = decision + (2 * (x - y)) + 1;
        }
    }
}

void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3) {
    drawLine(x1, y1, x2, y2);
    drawLine(x2, y2, x3, y3);
    drawLine(x3, y3, x1, y1);
}

const char *getTypeName(ObjectType type) {
    switch (type) {
        case LINE:
            return "Line";
        case RECTANGLE:
            return "Rectangle";
        case CIRCLE:
            return "Circle";
        case TRIANGLE:
            return "Triangle";
        default:
            return "Unknown";
    }
}

void drawObject(Object object) {
    if (!object.active) {
        return;
    }

    switch (object.type) {
        case LINE:
            drawLine(object.x1, object.y1, object.x2, object.y2);
            break;
        case RECTANGLE:
            drawRectangle(object.x1, object.y1, object.x2, object.y2);
            break;
        case CIRCLE:
            drawCircle(object.x1, object.y1, object.radius);
            break;
        case TRIANGLE:
            drawTriangle(object.x1, object.y1, object.x2, object.y2,
                         object.x3, object.y3);
            break;
        default:
            break;
    }
}

void redrawPicture(void) {
    int i;

    clearCanvas();

    for (i = 0; i < objectCount; i++) {
        drawObject(objects[i]);
    }
}

int findObjectIndexById(int id) {
    int i;

    for (i = 0; i < objectCount; i++) {
        if (objects[i].id == id && objects[i].active) {
            return i;
        }
    }

    return -1;
}

int findFreeObjectIndex(void) {
    int i;

    for (i = 0; i < objectCount; i++) {
        if (!objects[i].active) {
            return i;
        }
    }

    if (objectCount < MAX_OBJECTS) {
        return objectCount;
    }

    return -1;
}

void showObjectDetails(Object object) {
    printf("ID: %d | Type: %s | ", object.id, getTypeName(object.type));

    switch (object.type) {
        case LINE:
            printf("Start (%d,%d), End (%d,%d)\n",
                   object.x1, object.y1, object.x2, object.y2);
            break;
        case RECTANGLE:
            printf("Corner 1 (%d,%d), Corner 2 (%d,%d)\n",
                   object.x1, object.y1, object.x2, object.y2);
            break;
        case CIRCLE:
            printf("Center (%d,%d), Radius %d\n",
                   object.x1, object.y1, object.radius);
            break;
        case TRIANGLE:
            printf("Points (%d,%d), (%d,%d), (%d,%d)\n",
                   object.x1, object.y1, object.x2, object.y2,
                   object.x3, object.y3);
            break;
        default:
            printf("\n");
            break;
    }
}

void listObjects(void) {
    int i;
    int found = 0;

    printf("\nObjects in picture:\n");

    for (i = 0; i < objectCount; i++) {
        if (objects[i].active) {
            showObjectDetails(objects[i]);
            found = 1;
        }
    }

    if (!found) {
        printf("No objects are currently present.\n");
    }
}

ObjectType chooseObjectType(void) {
    int choice;

    printf("\nChoose object type:\n");
    printf("1. Line\n");
    printf("2. Rectangle\n");
    printf("3. Circle\n");
    printf("4. Triangle\n");

    if (!readInt("Enter choice: ", &choice)) {
        return 0;
    }

    if (choice < LINE || choice > TRIANGLE) {
        return 0;
    }

    return (ObjectType)choice;
}

int readObjectData(Object *object) {
    int maxRadius;

    object->x1 = 0;
    object->y1 = 0;
    object->x2 = 0;
    object->y2 = 0;
    object->x3 = 0;
    object->y3 = 0;
    object->radius = 0;

    printf("Use coordinates x = 0 to %d and y = 0 to %d.\n",
           COLS - 1, ROWS - 1);

    switch (object->type) {
        case LINE:
            if (!readCanvasX("Enter x1: ", &object->x1) ||
                !readCanvasY("Enter y1: ", &object->y1) ||
                !readCanvasX("Enter x2: ", &object->x2) ||
                !readCanvasY("Enter y2: ", &object->y2)) {
                return 0;
            }
            break;
        case RECTANGLE:
            if (!readCanvasX("Enter first corner x1: ", &object->x1) ||
                !readCanvasY("Enter first corner y1: ", &object->y1) ||
                !readCanvasX("Enter opposite corner x2: ", &object->x2) ||
                !readCanvasY("Enter opposite corner y2: ", &object->y2)) {
                return 0;
            }
            break;
        case CIRCLE:
            if (!readCanvasX("Enter center x: ", &object->x1) ||
                !readCanvasY("Enter center y: ", &object->y1)) {
                return 0;
            }

            maxRadius = minInt(object->x1, COLS - 1 - object->x1);
            maxRadius = minInt(maxRadius, object->y1);
            maxRadius = minInt(maxRadius, ROWS - 1 - object->y1);

            printf("Maximum radius for this center is %d.\n", maxRadius);
            if (!readBoundedInt("Enter radius: ", 0, maxRadius,
                                &object->radius)) {
                return 0;
            }
            break;
        case TRIANGLE:
            if (!readCanvasX("Enter x1: ", &object->x1) ||
                !readCanvasY("Enter y1: ", &object->y1) ||
                !readCanvasX("Enter x2: ", &object->x2) ||
                !readCanvasY("Enter y2: ", &object->y2) ||
                !readCanvasX("Enter x3: ", &object->x3) ||
                !readCanvasY("Enter y3: ", &object->y3)) {
                return 0;
            }
            break;
        default:
            return 0;
    }

    return 1;
}

void addObject(void) {
    int index;
    Object newObject;

    index = findFreeObjectIndex();

    if (index == -1) {
        printf("Object limit reached.\n");
        return;
    }

    newObject.type = chooseObjectType();

    if (newObject.type == 0) {
        printf("Invalid object type.\n");
        return;
    }

    newObject.id = nextId;
    nextId++;
    newObject.active = 1;

    if (!readObjectData(&newObject)) {
        printf("Add object cancelled.\n");
        nextId--;
        return;
    }

    objects[index] = newObject;
    if (index == objectCount) {
        objectCount++;
    }
    redrawPicture();

    printf("Object added successfully. ID = %d\n", newObject.id);
}

void deleteObject(void) {
    int id;
    int index;

    listObjects();
    if (!readInt("\nEnter object ID to delete: ", &id)) {
        printf("Delete object cancelled.\n");
        return;
    }

    index = findObjectIndexById(id);

    if (index == -1) {
        printf("Object not found.\n");
        return;
    }

    objects[index].active = 0;
    redrawPicture();

    printf("Object deleted successfully.\n");
}

void modifyObject(void) {
    int id;
    int index;
    ObjectType newType;
    Object modifiedObject;

    listObjects();
    if (!readInt("\nEnter object ID to modify: ", &id)) {
        printf("Modify object cancelled.\n");
        return;
    }

    index = findObjectIndexById(id);

    if (index == -1) {
        printf("Object not found.\n");
        return;
    }

    printf("\nCurrent object:\n");
    showObjectDetails(objects[index]);

    newType = chooseObjectType();

    if (newType == 0) {
        printf("Invalid object type.\n");
        return;
    }

    modifiedObject = objects[index];
    modifiedObject.type = newType;

    if (!readObjectData(&modifiedObject)) {
        printf("Modify object cancelled.\n");
        return;
    }

    objects[index] = modifiedObject;
    redrawPicture();

    printf("Object modified successfully.\n");
}

void clearPicture(void) {
    int i;

    for (i = 0; i < objectCount; i++) {
        objects[i].active = 0;
    }

    objectCount = 0;
    nextId = 1;
    clearCanvas();

    printf("Picture cleared successfully.\n");
}

void showMenu(void) {
    printf("\n===== 2D Graphics Editor =====\n");
    printf("1. Add Object\n");
    printf("2. Delete Object\n");
    printf("3. Modify Object\n");
    printf("4. Display Picture\n");
    printf("5. List Objects\n");
    printf("6. Clear Picture\n");
    printf("0. Exit\n");
}

int main(void) {
    int choice;

    clearCanvas();

    do {
        showMenu();
        if (!readInt("Enter your choice: ", &choice)) {
            printf("Exiting program.\n");
            break;
        }

        switch (choice) {
            case 1:
                addObject();
                break;
            case 2:
                deleteObject();
                break;
            case 3:
                modifyObject();
                break;
            case 4:
                displayPicture();
                break;
            case 5:
                listObjects();
                break;
            case 6:
                clearPicture();
                break;
            case 0:
                printf("Exiting program.\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
                break;
        }
    } while (choice != 0);

    return 0;
}
