var express = require('express');
var app = express();
app.use(express.static('public'))
var mongoose = require('mongoose');
var Posture = require("./PostureModel");

mongoose.connect('mongodb://admin:ese350@ds149820.mlab.com:49820/posture-data', function (err) {
  if (err) {
    console.log('Error connecting to mongodb database: %s.\nIs "mongod" running?', err.message);
    process.exit(0);
  }
});

var addPostureData = function (angle, callback) {
  var newPosture = new Posture({
    time: (new Date()).getTime(),
    angle: angle
  });
  console.log(newPosture);
  newPosture.save(function (err) {
    if (err) {
      console.log("Error saving posture data!");
      console.log(err);
    } else {
      callback();
    }   
  });
}

app.post('/addPosture', function (req, res) {
  addPostureData(req.query.angle, function() {
    res.send('Successfully added posture!' + " " + req.query.angle);
  });
  
})

app.get('/deleteAllPostures', function (req, res) {
  Posture.remove({}, function() {
    res.send('Successfully cleared postures!');
  })
})

app.get('/getPostures', function (req, res) {
  Posture.find(function (err, postures) {
    if (err) {
      return console.error(err);
    } else {
      res.send(postures);
    }
  });
});


app.listen(process.env.PORT || 3000, function () {
  console.log('App is running!')
})