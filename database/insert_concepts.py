import os
import psycopg2


def import_concepts_in_db(dataset_name:str=None, concepts:list=None, concepts_folder:str=None):
    '''
    Inserts all concepts for which a folder exists into the database in alphabetic order.

    :param dataset_name: Name of the dataset where concepts were extracted from.
    :param concepts: Uses list entries instead of folders as concept names, if not None.
    :param concepts_folder: The root folder to iterate through for concept folders.
    '''
    if dataset_name is None:
        raise ValueError("dataset_name is used as foreign key and must not be None.")

    conn = psycopg2.connect("dbname=postgres user=postgres password=contentsearch")
    cur = conn.cursor()

    cur.execute(f"SELECT * FROM concepts WHERE dataset_name = \'{dataset_name}\'")
    if cur.rowcount > 0:
        print(f"Already found entries for {dataset_name}, exiting.")
        return

    if concepts is None:
        # load all concepts based on folders for ImageDataGenerator.flow_from_directory, which labels the same way
        concepts = []
        for path, dirs, files in os.walk(concepts_folder):
            if dirs is None or len(dirs) == 0:
                continue
            concepts.extend(dirs)

    if len(concepts) == 0:
        print("Please prepare image paths in selected folder first.")
        return

    # add all concepts with id starting at 0
    for idx in range(len(concepts)):
        cur.execute(f"insert into concepts(id, name, dataset_name) values(\'{idx}\', \'{concepts[idx]}\', \'{dataset_name}\')")
    conn.commit()
    cur.close()
    print(f"Inserted concepts for {dataset_name}.")


if __name__ == '__main__':
    import_concepts_in_db(dataset_name='Mixed', concepts=['aeroplane' ,'bicycle' ,'buildings' ,'car' ,'forest' ,'glacier' ,'motorbike' ,'mountain' ,'sea' ,'street'])
    import_concepts_in_db(dataset_name='IIC', concepts_folder='IIC/seg_train/seg_train')
    import_concepts_in_db(dataset_name='VOC2012', concepts_folder='VOCdevkit/VOC2012/prepared/training')