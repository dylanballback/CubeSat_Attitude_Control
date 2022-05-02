/* globals Chart:false, feather:false */

(function () {
  'use strict'

  feather.replace({ 'aria-hidden': 'true' })

})()

// Graphs
var ctx = document.getElementById('myChart')
// eslint-disable-next-line no-unused-vars
var myChart = new Chart(ctx, {
  type: 'line',
  data: {
    datasets: [{
      label: "Z-Angle",
      data: [],
      lineTension: 0,
      backgroundColor: 'transparent',
      borderColor: 'red',
      borderWidth: 4,
      pointBackgroundColor: 'red'
    },{
      label: "Photoresistor 1",
      data: [],
      lineTension: 0,
      backgroundColor: 'transparent',
      borderColor: 'green',
      borderWidth: 4,
      pointBackgroundColor: 'green'
    },{
      data: [],
      label: "Photoresistor 2",
      lineTension: 0,
      backgroundColor: 'transparent',
      borderColor: 'blue',
      borderWidth: 4,
      pointBackgroundColor: 'blue'
    }],
    labels: ["X Acc","Y Acc","Z Acc"]
  },
  options: {
  plugins: {
    // Change options for ALL axes of THIS CHART
    streaming: {
      duration: 20000
    }
  },
  scales: {
    x: {
      type: 'realtime',
      // Change options only for THIS AXIS
      realtime: {
        duration: 20000
      }
    }
  }
}
})


const log = (text, color) => {
  var objDiv = document.getElementById("log");
  objDiv.innerHTML += `
  <tr>
    <td style="color: ${color}">${text}</td>
  </tr>
  `;
  
  var logTable = document.getElementById("logTable");
  logTable.scrollTop = logTable.scrollHeight;
};

const socket = new WebSocket('ws://' + "192.168.0.106:5678" + '/');
socket.addEventListener('message', ev => {
  const message = ev.data.split(":");
  //[type,otherdata]
  //[accelerationData, x, y, z]
  switch (message[0]) {
    case "accelerationData":
      myChart.data.datasets[0].data.push({
        x: Date.now(),
        y: message[1]
      });
      myChart.data.datasets[1].data.push({
        x: Date.now(),
        y: message[2]
      });
      myChart.data.datasets[2].data.push({
        x: Date.now(),
        y: message[3]
      });
      myChart.update('quiet');

      break;
    case "BatteryData":

      document.getElementById('battery_1').innerHTML = message[1];
      document.getElementById('battery_2').innerHTML = message[2];
      document.getElementById('battery_3').innerHTML = message[3];
      document.getElementById('temperature').innerHTML = message[4];

      break;
    default:
      log('<<< ' + ev.data, 'blue');
  }
});
socket.addEventListener('close', ev => {
  log('<<< closed');
});
document.getElementById('socketInput').onsubmit = ev => {
  ev.preventDefault();
  const textField = document.getElementById('socketTextInput');
  log('>>> ' + textField.value, 'red');
  socket.send(textField.value);
  textField.value = '';
};