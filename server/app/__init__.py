import logging
from flask import Flask
from flask_bootstrap import Bootstrap

bootstrap = Bootstrap()

def create_app():

    app = Flask(__name__)
    
    app.config['SECRET_KEY'] = 'smartHouseAVR20'
    bootstrap.init_app(app)

    from app.errors import bp as errors_bp
    app.register_blueprint(errors_bp)

    from app.main import bp as main_bp
    app.register_blueprint(main_bp)

    app.logger.setLevel(logging.INFO)
    app.logger.info('SmartHouse startup')

    return app
