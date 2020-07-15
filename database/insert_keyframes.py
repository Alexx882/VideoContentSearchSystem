import psycopg2
import os


def path_normjoin(p1: str, p2: str) -> str:
    return os.path.normpath(os.path.join(p1, p2))


def load_videos():
    videos = []
    for path, _, files in os.walk('videos/v3c1/'):
        videos = [
                    {'id': f[:5], 'path': path_normjoin(path, f)}
                    for f in files 
                    if '.mp4' in f
                ]
    return videos


def load_keyframes():
    keyframes = []
    for path, _, files in os.walk('keyframes/v3c1/'):
        for f in files:
            if '.png' not in f:
                continue

            path_to_file = path_normjoin(path, f)
            video_id = path_to_file.split(os.path.normpath('/'))[-2]
            id_ = f"{video_id}_{path_to_file.split(os.path.normpath('/'))[-1].split('.')[0]}"
                            
            keyframes.append({
                'id': id_, 
                'path': path_to_file,
                'video_id': video_id
                })
    return keyframes


if __name__ == '__main__':
    conn = psycopg2.connect("dbname=postgres user=postgres password=contentsearch")
    cur = conn.cursor()

    cur.execute("SELECT * FROM videos")
    if cur.rowcount > 0:
        print("Already found entries, exiting.")
        exit(0)

    videos = load_videos()
    keyframes = load_keyframes()

    for video in videos:
        cur.execute(f"insert into videos(id, filepath) values(\'{video['id']}\', \'{video['path']}\')")
    print("Inserted videos.")

    for kf in keyframes:
        cur.execute(f"insert into keyframes(id, filepath, video_id) values(\'{kf['id']}\', \'{kf['path']}\', \'{kf['video_id']}\')")
    print("Inserted keyframes.")

    conn.commit()
    cur.close()
