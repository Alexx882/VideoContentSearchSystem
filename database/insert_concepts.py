import os
import psycopg2

CONCEPTS_FOLDER = 'IIC/seg_train/seg_train'

def import_concepts_in_db(concepts:list=None):
    '''
    Inserts all concepts for which a folder exists into the database in alphabetic order.
    :param concepts: Uses list entries instead of folders as concept names, if not None.
    '''
    conn = psycopg2.connect("dbname=postgres user=postgres password=contentsearch")
    cur = conn.cursor()

    cur.execute("SELECT * FROM concepts")
    if cur.rowcount > 0:
        print("Already found entries, exiting.")
        return

    # load all concepts based on folders for ImageDataGenerator.flow_from_directory, which labels the same way
    if concepts is None:
        concepts = []
        for path, dirs, files in os.walk(CONCEPTS_FOLDER):
            if dirs is None or len(dirs) == 0:
                continue
            concepts.extend(dirs)

    if len(concepts) == 0:
        print("Please prepare image paths first. (Manually call copy_images_to_labeled_folder() in this script.)")
        return

    # add all concepts with id starting at 0
    for idx in range(len(concepts)):
        cur.execute(f"insert into concepts(id, name) values(\'{idx}\', \'{concepts[idx]}\')")
    conn.commit()
    cur.close()
    print("Inserted concepts.")


if __name__ == '__main__':
    import_concepts_in_db(['aeroplane' ,'bicycle' ,'buildings' ,'car' ,'forest' ,'glacier' ,'motorbike' ,'mountain' ,'sea' ,'street'])