from flask_wtf import FlaskForm
from wtforms import StringField, SubmitField
from wtforms.validators import DataRequired, Length

class DeviceNameForm(FlaskForm):
    deviceName = StringField('Nuovo nome:', validators=[DataRequired(), Length(1,20)])
    submit = SubmitField('Applica')
    
class ChannelNameForm(FlaskForm):
    channelName = StringField('Nuovo nome:', validators=[DataRequired(), Length(1,20)])
    submit = SubmitField('Applica')