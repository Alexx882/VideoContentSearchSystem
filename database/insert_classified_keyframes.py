import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '1'

import tensorflow as tf
from tensorflow import keras
from tensorflow.keras.preprocessing import image
import numpy as np
from typing import List, Dict
import psycopg2

import insert_keyframes as kf_helper

IMG_HEIGHT = 256
IMG_WIDTH = 256
IMG_CHANNELS = 3


def convert_to_classification_result(keyframe_id, prediction: np.ndarray, confidence_treshold=0.02):
    res = []
    for i in range(len(prediction)):
        if prediction[i] > confidence_treshold:
            res.append({
                'keyframe_id': keyframe_id, 
                'concept_id': i,
                'confidence': prediction[i]
            })
    return res


def predict_all_keyframes() -> List[Dict]:
    '''
    Returns a list with prediction results of all keyframes. 
    The results contain keyframe id, concept id and confidence.
    '''
    keyframes = kf_helper.load_keyframes()
    model = keras.models.load_model('ImageClassifier/voc2012.h5')

    keyframe_concepts = []

    for keyframe in keyframes:
        img = image.img_to_array(image.load_img(keyframe['path'], target_size=(IMG_HEIGHT,IMG_WIDTH)))
        img = img / 255.
        img = np.array([img]) # Convert single image to a batch
        res = model.predict(img)[0] # Convert batch result to single result

        keyframe_concepts.extend(convert_to_classification_result(keyframe['id'], res))
    
    return keyframe_concepts


if __name__ == '__main__':
    conn = psycopg2.connect("dbname=postgres user=postgres password=contentsearch")
    cur = conn.cursor()

    cur.execute("SELECT * FROM keyframe_concepts")
    if cur.rowcount > 0:
        print("Already found entries, exiting.")
        exit(0)

    predictions = predict_all_keyframes()
    for prediction in predictions:
        cur.execute(f"insert into keyframe_concepts(keyframe_id, concept_id, confidence) " \
                    f"values(\'{prediction['keyframe_id']}\', \'{prediction['concept_id']}\', \'{prediction['confidence']}\')")    
    conn.commit()
    cur.close()
    print("Inserted keyframe-concepts.")