import flask
from repo import Repo

app = flask.Flask(__name__, static_url_path='/static')

repo = Repo()

@app.route('/')
def hello_world():
    concepts = repo.get_concepts()
    datasets = repo.get_datasets()
    return flask.render_template("index.html", concept_list=concepts, dataset_list=datasets)

@app.route('/results')
def get_concepts():
    dataset = flask.request.args.get('pdataset')
    concept_id = flask.request.args.get('pconcept')
    confidence = int(flask.request.args.get('pconf')) / 100 # input in %

    # load concept name 
    concept = [concept[1] for concept in repo.get_concepts() 
               if concept[0] == concept_id and concept[2] == dataset]
    if len(concept) != 1:
        return flask.Response('Error 500 - Inconsistent database entries for selected database and concept.', 500)
    concept = concept[0]
    
    keyframes = repo.get_keyframes(dataset, concept_id, confidence)
    
    return flask.render_template("keyframe_results.html", dataset=dataset, concept=concept, keyframes=keyframes)

app.run(host='0.0.0.0', port=5000)