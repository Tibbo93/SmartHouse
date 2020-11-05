from flask import render_template, request
from app.main import bp
from app.utilities import c_lib, Conf

@bp.route('/', methods=['GET'])
@bp.route('/index', methods=['GET'])
def index():
    return render_template('index.html', switch_B=Conf.switch_B)

@bp.route('/set_channel_value/<channel>', methods=['PUT'])
def set_channel_value(channel):
    req_data = request.get_json()
    str = "set_channel_value"
    str = "{0} {1} {2}".format(str, channel, req_data["value"])
    c_lib.perform(bytes(str, encoding="utf-8"), Conf.serial_port)
    return render_template('index.html'), 200
