var mongoose = require('mongoose');

var Schema = mongoose.Schema;

var forwardPostureModel = new Schema({
  time: Number,
  angle: Number
});

var Posture = mongoose.model('ForwardPosture', forwardPostureModel);

module.exports = Posture;