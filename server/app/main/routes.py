from flask import render_template, request, redirect, url_for
from app.main import bp
from app.utilities import c_lib, Conf
import app.forms as forms

@bp.route('/', methods=['GET'])
@bp.route('/index', methods=['GET'])
def index():
    return render_template('index.html', switch_B=Conf.switch_B, switch_L=Conf.switch_L,
                        digital_in=Conf.digital_in, analog_in=Conf.analog_in)

@bp.route('/set_channel_value/<channel>', methods=['PUT'])
def set_channel_value(channel):
    req_data = request.get_json()
    str = "set_channel_value"
    str = "{0}:{1}:{2}".format(str, channel, req_data["value"])
    c_lib.perform(bytes(str, encoding="utf-8"), Conf.serial_port)
    return render_template('index.html'), 200

@bp.route('/settings', methods=['GET'])
def settings():
    deviceName = Conf.name
    return render_template('settings.html', deviceName=deviceName, switch_B=Conf.switch_B,
                        switch_L=Conf.switch_L, digital_in=Conf.digital_in, analog_in=Conf.analog_in)

@bp.route('/edit-settings', methods=['GET'])
def edit_settings():
    deviceName = Conf.name
    deviceNameForm = forms.DeviceNameForm()
    channelNameForm = forms.ChannelNameForm()
    return render_template('edit-settings.html', deviceNameForm=deviceNameForm, channelNameForm=channelNameForm,
                        deviceName=deviceName, switch_B=Conf.switch_B, switch_L=Conf.switch_L, digital_in=Conf.digital_in,
                        analog_in=Conf.analog_in)
    
@bp.route('/edit-settings/<cmd>', methods=['POST'])
def set_name(cmd):
    deviceNameForm = forms.DeviceNameForm()
    newDeviceName = deviceNameForm.deviceName.data
    if deviceNameForm.validate_on_submit():
        Conf.name = newDeviceName
        str = "{0}:{1}".format(cmd, newDeviceName.replace(' ', '_'))
        c_lib.perform(bytes(str, encoding="utf-8"), Conf.serial_port)
        deviceNameForm.deviceName.data = ''
    return redirect(url_for('main.edit_settings'))
    
@bp.route('/edit-settings/<cmd>:<device>:<channel>', methods=['POST'])
def set_channel_name(cmd, device, channel):
    channelNameForm = forms.ChannelNameForm()
    newChannelName = channelNameForm.channelName.data.replace(' ', '_')
    if channelNameForm.validate_on_submit():
        Conf.set_channel_name(channel, newChannelName)
        str = "{0}:{1}:{2}:{3}".format(cmd, device.replace(' ', '_'), channel, newChannelName)
        c_lib.perform(bytes(str, encoding="utf-8"), Conf.serial_port)
        channelNameForm.channelName.data = ''
    return redirect(url_for('main.edit_settings'))