import flask
from repo import Repo

app = flask.Flask(__name__, static_url_path='/static')

repo = Repo()

@app.route('/')
def hello_world():
    concepts = repo.get_concepts()
    return flask.render_template("index.html", concept_list=concepts)

@app.route('/results')
def get_concepts():
    concept_id = flask.request.args.get('pconcept')
    confidence = int(flask.request.args.get('pconf')) / 100 # input in %
    print(f"request: {concept_id}, {confidence}")

    concept = [concept[1] for concept in repo.get_concepts() if concept[0] == concept_id][0]
    keyframes = repo.get_keyframes(concept_id, confidence)
    
    return flask.render_template("keyframe_results.html", concept=concept, keyframes=keyframes)

app.run()