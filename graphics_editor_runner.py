import math

ROWS = 25
COLS = 60
MAX_OBJECTS = 50
EMPTY = "_"
DRAW = "*"

LINE = 1
RECTANGLE = 2
CIRCLE = 3
TRIANGLE = 4

canvas = [[EMPTY for _ in range(COLS)] for _ in range(ROWS)]
objects = []
next_id = 1


def read_ints(prompt, count):
    while True:
        try:
            values = [int(value) for value in input(prompt).split()]
        except ValueError:
            print("Please enter numbers only.")
            continue

        if len(values) == count:
            return values

        print(f"Please enter exactly {count} numbers.")


def clear_canvas():
    for y in range(ROWS):
        for x in range(COLS):
            canvas[y][x] = EMPTY


def plot_point(x, y):
    if 0 <= x < COLS and 0 <= y < ROWS:
        canvas[y][x] = DRAW


def display_canvas():
    print()
    for row in canvas:
        print("".join(row))
    print()


def draw_line(x1, y1, x2, y2):
    dx = abs(x2 - x1)
    dy = abs(y2 - y1)
    sx = 1 if x1 < x2 else -1
    sy = 1 if y1 < y2 else -1
    err = dx - dy

    while True:
        plot_point(x1, y1)
        if x1 == x2 and y1 == y2:
            break

        e2 = 2 * err
        if e2 > -dy:
            err -= dy
            x1 += sx
        if e2 < dx:
            err += dx
            y1 += sy


def draw_rectangle(x1, y1, x2, y2):
    draw_line(x1, y1, x2, y1)
    draw_line(x2, y1, x2, y2)
    draw_line(x2, y2, x1, y2)
    draw_line(x1, y2, x1, y1)


def draw_circle(cx, cy, radius):
    for angle in range(360):
        rad = angle * math.pi / 180.0
        x = cx + int(radius * math.cos(rad))
        y = cy + int(radius * math.sin(rad))
        plot_point(x, y)


def draw_triangle(x1, y1, x2, y2, x3, y3):
    draw_line(x1, y1, x2, y2)
    draw_line(x2, y2, x3, y3)
    draw_line(x3, y3, x1, y1)


def draw_object(obj):
    if not obj["active"]:
        return

    if obj["type"] == LINE:
        draw_line(obj["x1"], obj["y1"], obj["x2"], obj["y2"])
    elif obj["type"] == RECTANGLE:
        draw_rectangle(obj["x1"], obj["y1"], obj["x2"], obj["y2"])
    elif obj["type"] == CIRCLE:
        draw_circle(obj["x1"], obj["y1"], obj["radius"])
    elif obj["type"] == TRIANGLE:
        draw_triangle(obj["x1"], obj["y1"], obj["x2"], obj["y2"], obj["x3"], obj["y3"])


def redraw_all_objects():
    clear_canvas()
    for obj in objects:
        draw_object(obj)


def type_name(obj_type):
    return {
        LINE: "Line",
        RECTANGLE: "Rectangle",
        CIRCLE: "Circle",
        TRIANGLE: "Triangle",
    }.get(obj_type, "Unknown")


def list_objects():
    print("\nObjects in picture:")
    visible = False
    for obj in objects:
        if obj["active"]:
            visible = True
            print(f"ID: {obj['id']} | Type: {type_name(obj['type'])}")
    if not visible:
        print("No objects.")


def find_object_by_id(object_id):
    for obj in objects:
        if obj["id"] == object_id and obj["active"]:
            return obj
    return None


def add_object():
    global next_id

    if len(objects) >= MAX_OBJECTS:
        print("Object limit reached!")
        return

    print("\nChoose object type:")
    print("1. Line")
    print("2. Rectangle")
    print("3. Circle")
    print("4. Triangle")

    try:
        obj_type = int(input("Enter choice: "))
    except ValueError:
        print("Invalid object type!")
        return

    obj = {"id": next_id, "type": obj_type, "active": True}

    if obj_type == LINE:
        obj["x1"], obj["y1"], obj["x2"], obj["y2"] = read_ints("Enter x1 y1 x2 y2: ", 4)
    elif obj_type == RECTANGLE:
        obj["x1"], obj["y1"], obj["x2"], obj["y2"] = read_ints(
            "Enter top-left x1 y1 and bottom-right x2 y2: ", 4
        )
    elif obj_type == CIRCLE:
        obj["x1"], obj["y1"], obj["radius"] = read_ints("Enter center x y and radius: ", 3)
    elif obj_type == TRIANGLE:
        (
            obj["x1"],
            obj["y1"],
            obj["x2"],
            obj["y2"],
            obj["x3"],
            obj["y3"],
        ) = read_ints("Enter x1 y1 x2 y2 x3 y3: ", 6)
    else:
        print("Invalid object type!")
        return

    objects.append(obj)
    next_id += 1
    redraw_all_objects()
    print(f"Object added successfully with ID {obj['id']}.")


def delete_object():
    list_objects()
    try:
        object_id = int(input("\nEnter object ID to delete: "))
    except ValueError:
        print("Object not found!")
        return

    obj = find_object_by_id(object_id)
    if obj is None:
        print("Object not found!")
        return

    obj["active"] = False
    redraw_all_objects()
    print("Object deleted successfully.")


def modify_object():
    list_objects()
    try:
        object_id = int(input("\nEnter object ID to modify: "))
    except ValueError:
        print("Object not found!")
        return

    obj = find_object_by_id(object_id)
    if obj is None:
        print("Object not found!")
        return

    print("\nEnter new details for this object.")
    if obj["type"] == LINE:
        obj["x1"], obj["y1"], obj["x2"], obj["y2"] = read_ints("Enter new x1 y1 x2 y2: ", 4)
    elif obj["type"] == RECTANGLE:
        obj["x1"], obj["y1"], obj["x2"], obj["y2"] = read_ints(
            "Enter new top-left x1 y1 and bottom-right x2 y2: ", 4
        )
    elif obj["type"] == CIRCLE:
        obj["x1"], obj["y1"], obj["radius"] = read_ints("Enter new center x y and radius: ", 3)
    elif obj["type"] == TRIANGLE:
        (
            obj["x1"],
            obj["y1"],
            obj["x2"],
            obj["y2"],
            obj["x3"],
            obj["y3"],
        ) = read_ints("Enter new x1 y1 x2 y2 x3 y3: ", 6)

    redraw_all_objects()
    print("Object modified successfully.")


def main():
    clear_canvas()

    while True:
        print("\n===== 2D Graphics Editor =====")
        print("1. Add Object")
        print("2. Delete Object")
        print("3. Modify Object")
        print("4. Display Picture")
        print("5. List Objects")
        print("6. Clear Picture")
        print("0. Exit")

        try:
            choice = int(input("Enter your choice: "))
        except ValueError:
            print("Invalid choice! Try again.")
            continue

        if choice == 1:
            add_object()
        elif choice == 2:
            delete_object()
        elif choice == 3:
            modify_object()
        elif choice == 4:
            display_canvas()
        elif choice == 5:
            list_objects()
        elif choice == 6:
            for obj in objects:
                obj["active"] = False
            clear_canvas()
            print("Picture cleared successfully.")
        elif choice == 0:
            print("Exiting program...")
            break
        else:
            print("Invalid choice! Try again.")


if __name__ == "__main__":
    main()
