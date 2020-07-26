import psycopg2

def insert_tables():
    # docker run --name contentsearch -e POSTGRES_PASSWORD=contentsearch -e PGPASSWORD=contentsearch -p 5432:5432 -d postgres
    conn = psycopg2.connect("dbname=postgres user=postgres password=contentsearch")

    create_table_commands = [
    """
    create table videos(
        id VARCHAR(255) PRIMARY KEY NOT NULL,
        filepath VARCHAR(255)
    );
    """,

    """
    create table keyframes(
        id VARCHAR(255) PRIMARY KEY NOT NULL,
        filepath VARCHAR(255),
        video_id VARCHAR(255)
    );
    """,

    """
    create table concepts(
        id VARCHAR(255) NOT NULL,
        name VARCHAR(255),
        dataset_name VARCHAR(255),
        
	    PRIMARY KEY (id, dataset_name)
    );
    """,

    """
    create table keyframe_concepts(
        id SERIAL PRIMARY KEY NOT NULL,
        keyframe_id VARCHAR(255),
        concept_id VARCHAR(255),
        confidence DECIMAL(5,4),
        dataset_name VARCHAR(255)        
    );
    """,

    """
    create table datasets(
        name VARCHAR(255) PRIMARY KEY NOT NULL,
        description text
    );
    """
    ]

    cursor = conn.cursor()
    for command in create_table_commands:
        cursor.execute(command)

    conn.commit()
    cursor.close()
    

if __name__ == '__main__':
    insert_tables()
    print('Created tables.')
