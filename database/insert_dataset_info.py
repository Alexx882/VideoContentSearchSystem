import psycopg2

def get_descriptions():
    return \
        {'VOC2012':
            'The Visual Object Classes Challenge 2012 dataset contains images of partially rather small objects, is not balanced, and contains multiple labels per image. '
            'Only the first label was used as concept.',
         'IIC': 
            'The Intel Image Classification dataset contains mainly landscapes but has only six classes in total.',
         'Mixed': 
            "This dataset contains all six classes from IIC and additionally four classes from VOC2012."
         }


def insert_descriptions():
    conn = psycopg2.connect("dbname=postgres user=postgres password=contentsearch")
    cursor = conn.cursor()

    cursor.execute("SELECT * FROM datasets")
    if cursor.rowcount > 0:
        print("Already found entries, exiting.")
        return

    for (name, desc) in get_descriptions().items():
        cursor.execute(f"insert into datasets(name, description) values(\'{name}\', \'{desc}\')")

    conn.commit()
    cursor.close()

    print("Inserted datasets.")


if __name__ == '__main__':
    insert_descriptions()