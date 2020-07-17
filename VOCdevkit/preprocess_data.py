import os
import shutil
import xml.etree.ElementTree as ET

import psycopg2

def path_normjoin(p1: str, p2: str) -> str:
    return os.path.normpath(os.path.join(p1, p2))

BASE_PATH = os.path.normpath("./VOCdevkit/VOC2012")
PREPARED_IMAGES_PATH = path_normjoin(BASE_PATH, 'prepared')


def copy_images_to_labeled_folder():
    '''
    This script preprocesses the data from VOCdevkit (http://host.robots.ox.ac.uk/pascal/VOC/voc2012/index.html#devkit).
    It moves images from the same class into the corresponding folder for processing with ImageDataGenerator.flow_from_directory.
    '''
    data = {}
    print("Loading images")
    for path, _, files in os.walk(path_normjoin(BASE_PATH, 'JPEGImages')):
        for image_file in files:

            # load first label from annotation xml
            xml_doc = ET.parse(path_normjoin(path_normjoin(BASE_PATH, 'Annotations'), image_file.replace('.jpg', '.xml')))
            label = xml_doc.findall('object')[0].findall('name')[0].text

            if label not in data:
                data[label] = []
            data[label].append(image_file)

    cnt = 0
    for label in data.keys():
        print(f"Copying images for {label} (concept {cnt+1} of {len(data.keys())})")
        cnt += 1

        # move files to new folder based on label
        training_folder = path_normjoin(path_normjoin(PREPARED_IMAGES_PATH, 'training'), label)
        test_folder = path_normjoin(path_normjoin(PREPARED_IMAGES_PATH, 'test'), label)
        if not os.path.exists(training_folder):
            os.makedirs(training_folder)
            os.makedirs(test_folder)

        num_train_data = int(len(data[label])*.7)

        for image_file in data[label][:num_train_data]:
            shutil.copy(path_normjoin(path, image_file), path_normjoin(training_folder, image_file))
            
        for image_file in data[label][num_train_data:]:
            shutil.copy(path_normjoin(path, image_file), path_normjoin(test_folder, image_file))


def import_concepts_in_db():
    '''Inserts all concepts for which folders exist into the database.'''
    conn = psycopg2.connect("dbname=postgres user=postgres password=contentsearch")
    cur = conn.cursor()

    cur.execute("SELECT * FROM concepts")
    if cur.rowcount > 0:
        print("Already found entries, exiting.")
        return

    # load all concepts based on folders for ImageDataGenerator.flow_from_directory, which labels the same way
    concepts = []
    for path, dirs, files in os.walk(path_normjoin(PREPARED_IMAGES_PATH, 'training')):
        if dirs is None or len(dirs) == 0:
            continue
        concepts.extend(dirs)

    if len(concepts) == 0:
        print("Please prepare image paths first. (Manually call copy_images_to_labeled_folder() in this script.)")
        return

    # add all concepts with id starting at 0
    for idx in range(len(concepts)):
        cur.execute(f"insert into concepts(id, name) values(\'{idx}\', \'{concepts[idx]}\')")
    print("Inserted concepts.")

    conn.commit()
    cur.close()


if __name__ == "__main__":
    # copy_images_to_labeled_folder()
    import_concepts_in_db()