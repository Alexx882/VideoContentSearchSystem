import psycopg2
import os
from typing import List, Tuple

# try to load db hostname from docker env
try:
    db_hostname = os.environ['CONTENT_SEARCH_DB_HOSTNAME']
except KeyError: 
    db_hostname = 'localhost'

class Repo:
    def __init__(self):
        self.db = psycopg2.connect(f"host={db_hostname} dbname=postgres user=postgres password=contentsearch")

    def get_concepts(self) -> List[Tuple]:
        '''Returns a list of concepts as (id, name).'''
        cur = self.db.cursor()
        cur.execute("SELECT id, name FROM concepts")
        return list(cur)

    def get_keyframes(self, concept_id, confidence) -> List[Tuple]:
        '''Returns a list of keyframes for concept_id with >= given confidence as (keyframe_id, keyframe_filepath, video_filepath, confidence).'''
        cur = self.db.cursor()
        cur.execute(
            f"""
            select k.id, k.filepath, v.filepath, kc.confidence from keyframe_concepts kc
                join keyframes k on k.id = kc.keyframe_id
                join videos v on v.id = k.video_id
            where kc.concept_id = '{concept_id}'
                and kc.confidence >= {confidence}
            """
            )
        return list(cur)
