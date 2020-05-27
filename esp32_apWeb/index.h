const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
  <title>GPS Unit</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="stylesheet" href="https://www.w3schools.com/w3css/4/w3.css">
<body>

  <div class="w3-card-4">

    <header class="w3-container w3-blue">
      <h1>GPS Unit status</h1>
    </header>
    
    <footer class="w3-container w3-blue">
      <h5>Serial1</h5>
    </footer>
    
    <div class="w3-container">
      <ul id="list1">
        <li>No data</li>
        </ul>
    </div>
    
    <footer class="w3-container w3-blue">
      <h5>Serial 2</h5>
    </footer>
    
    <div class="w3-container">
      <ul id="list2">
        <li>No data</li>
        </ul>
    </div>
    
    </div>

<script>

setInterval(function() {
  getData();
}, 2000); //2000mSeconds update rate

function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      
      var xmlDoc = this.responseXML;
      var serial1 = xmlDoc.getElementsByTagName("serial1");
      var serial2 = xmlDoc.getElementsByTagName("serial2");

      if (serial1.length == 1)
        {
      var node1 = document.createElement("LI");                 // Create a <li> node
      var textnode1 = document.createTextNode(serial1[0].innerHTML);
      node1.appendChild(textnode1);                              // Append the text to <li>
      document.getElementById("list1").appendChild(node1);
        }
        if (serial2.length == 1)
        {
      var node2 = document.createElement("LI");                 // Create a <li> node
      var textnode2 = document.createTextNode(serial2[0].innerHTML);
      node2.appendChild(textnode2);                              // Append the text to <li>
      document.getElementById("list2").appendChild(node2);
        }
    }
  };
  xhttp.open("GET", "readADC", true);
  xhttp.send();
}
</script>
</body>
</html>
)=====";
