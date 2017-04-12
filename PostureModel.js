var mongoose = require('mongoose');

var Schema = mongoose.Schema;

var postureModel = new Schema({
  time: Number,
  angle: Number
});

var Posture = mongoose.model('Posture', postureModel);

module.exports = Posture;