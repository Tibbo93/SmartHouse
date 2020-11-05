from app import create_app
from app.utilities import Conf

app = create_app()

Conf.conf_init(app)
