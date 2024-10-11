/*
 * simpleviewer and streamviewer
 */

const uint8_t index_simple_html[] = R"=====(<!doctype html>
<html>
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width,initial-scale=1">
    <title id="title">V4.1</title>
    <link rel="icon" type="image/png" sizes="32x32" href="/favicon-32x32.png">
    <link rel="icon" type="image/png" sizes="16x16" href="/favicon-16x16.png">
    <link rel="stylesheet" type="text/css" href="/style.css">
    <style>
      @media (min-width: 800px) and (orientation:landscape) {
        #content {
          display:flex;
          flex-wrap: nowrap;
          flex-direction: column;
          align-items: flex-start;
        }
      }
    </style>
  </head>

  <body>
    <section class="main">
      <div id="logo">
        <label for="nav-toggle-cb" id="nav-toggle" style="float:left;" title="Settings">&#9776;&nbsp;V4.1 con servos</label>
        <button id="swap-viewer" style="float:left;" title="Swap to full feature viewer">Full</button>
        <button id="get-still" style="float:left;">Foto</button>
        <button id="toggle-stream" style="float:left;" class="hidden">V&iacute;deo</button>
        <div id="wait-settings" style="float:left;" class="loader" title="Waiting for camera settings to load"></div>
      </div>
      <div id="content">
        <div class="hidden" id="sidebar">
          <input type="checkbox" id="nav-toggle-cb">
            <nav id="menu" style="width:24em;">
<!--  Cambio  -->
              <div class="input-group" id="servo1-group" >
                <label for="servo1">servo horizontal</label>
                <div class="range-min">◄</div>			
                  <input type="range" id="servo1" min="0" max="180" value="0" class="action-setting">
                <div class="range-max">►</div> 
              </div>

              <div class="input-group" id="servo2-group" >
                <label for="servo2">servo vertical</label>
                <div class="range-min">▲</div>			
                  <input type="range" id="servo2" min="0" max="180" value="0" class="action-setting">
                <div class="range-max">▼</div> 
              </div>
<!-- Fin cambio  -->
              <div class="input-group hidden" id="lamp-group" title="Flashlight LED.&#013;&#013;Warning:&#013;Built-In lamps can be Very Bright! Avoid looking directly at LED&#013;Can draw a lot of power and may cause visual artifacts, affect WiFi or even brownout the camera on high settings">
                 <label for="lamp">Light</label>
                <div class="range-min">Off</div>
                <input type="range" id="lamp" min="0" max="100" value="0" class="action-setting">
                <div class="range-max">Full&#9888;</div>
              </div>
              <div class="input-group" id="rotate-group">
                <label for="rotate">Rotate in Browser</label>
                <select id="rotate" class="default-action">
                  <option value="90">90&deg; (Right)</option>
                  <option value="0" selected="selected">0&deg; (None)</option>
                  <option value="-90">-90&deg; (Left)</option>
                </select>
              </div>


              <div class="input-group" id="framesize-group">
                <label for="framesize">Resolution</label>
                <select id="framesize" class="action-setting">
                  <option value="13">UXGA (1600x1200)</option>
                  <option value="12">SXGA (1280x1024)</option>
                  <option value="11">HD (1280x720)</option>
                  <option value="10">XGA (1024x768)</option>
                  <option value="9">SVGA (800x600)</option>
                  <option value="8">VGA (640x480)</option>
                  <option value="7">HVGA (480x320)</option>
                  <option value="6">CIF (400x296)</option>
                  <option value="5">QVGA (320x240)</option>
                  <option value="3">HQVGA (240x176)</option>
                  <option value="1">QQVGA (160x120)</option>
                  <option value="0">THUMB (96x96)</option>
                </select>
              </div>
              <!-- Hide the next entries, they are present in the body so that we
                  can pass settings to/from them for use in the scripting, not for user setting -->
              <div id="rotate" class="action-setting hidden"></div>
              <div id="cam_name" class="action-setting hidden"></div>
              <div id="stream_url" class="action-setting hidden"></div>
            </nav>
        </div>
        <figure>
          <div id="stream-container" class="image-container hidden">
          <a id="save-still" href="#" class="button save" download="capture.jpg">Guardar</a>
            <div class="close close-rot-none" id="close-stream">×</div>
            <img id="stream" src="" crossorigin>
          </div>
        </figure>
      </div>
    </section>
  </body>

  <script>
  document.addEventListener('DOMContentLoaded', function (event) {
    var baseHost = document.location.origin;
    var streamURL = 'Undefined';

    const settings = document.getElementById('sidebar')
    const waitSettings = document.getElementById('wait-settings')
    const lampGroup = document.getElementById('lamp-group')
    const servo1Group = document.getElementById('servo1-group')  /* cambio */
    const servo2Group= document.getElementById('servo2-group') /* cambio */
     const saveButton = document.getElementById('save-still')  /* cambio */
   
    const rotate = document.getElementById('rotate')
    const view = document.getElementById('stream')
    const viewContainer = document.getElementById('stream-container')
    const stillButton = document.getElementById('get-still')
    const streamButton = document.getElementById('toggle-stream')
    const closeButton = document.getElementById('close-stream')
    const swapButton = document.getElementById('swap-viewer')

    const hide = el => {
      el.classList.add('hidden')
    }
    const show = el => {
      el.classList.remove('hidden')
    }

    const disable = el => {
      el.classList.add('disabled')
      el.disabled = true
    }

    const enable = el => {
      el.classList.remove('disabled')
      el.disabled = false
    }

    const updateValue = (el, value, updateRemote) => {
      updateRemote = updateRemote == null ? true : updateRemote
      let initialValue
      if (el.type === 'checkbox') {
        initialValue = el.checked
        value = !!value
        el.checked = value
      } else {
        initialValue = el.value
        el.value = value
      }

      if (updateRemote && initialValue !== value) {
        updateConfig(el);
      } else if(!updateRemote){
        if(el.id === "lamp"){
          if (value == -1) {
            hide(lampGroup)
          } else {
            show(lampGroup)
          }
/*cambio */
        } else if(el.id === "servo1"){
          if (value == -1) {
            hide(servo1Group)
          } else {
            show(servo1Group)
          }
                  
        } else if(el.id === "servo2"){
          if (value == -1) {
            hide(servo2Group)
          } else {
            show(servo2Group)
          }
        

/*fin cambio */

        } else if(el.id === "cam_name"){
          window.document.title = value;
          console.log('Name set to: ' + value);
        } else if(el.id === "code_ver"){
          console.log('Firmware Build: ' + value);
        } else if(el.id === "rotate"){
          rotate.value = value;
          applyRotation();
        } else if(el.id === "stream_url"){
          streamURL = value;
          streamButton.setAttribute("title", `Start the stream :: {streamURL}`);
          console.log('Stream URL set to:' + value);
        }
      }
    }

    var rangeUpdateScheduled = false
    var latestRangeConfig

    function updateRangeConfig (el) {
      latestRangeConfig = el
      if (!rangeUpdateScheduled) {
        rangeUpdateScheduled = true;
        setTimeout(function(){
          rangeUpdateScheduled = false
          updateConfig(latestRangeConfig)
        }, 150);
      }
    }

    function updateConfig (el) {
      let value
      switch (el.type) {
        case 'checkbox':
          value = el.checked ? 1 : 0
          break
        case 'range':
        case 'select-one':
          value = el.value
          break
        case 'button':
        case 'submit':
          value = '1'
          break
        default:
          return
      }

      const query = `${baseHost}/control?var=${el.id}&val=${value}`

      fetch(query)
        .then(response => {
          console.log(`request to ${query} finished, status: ${response.status}`)
        })
    }

    document
      .querySelectorAll('.close')
      .forEach(el => {
        el.onclick = () => {
          hide(el.parentNode)
        }
      })

    // read initial values
    fetch(`${baseHost}/status`)
      .then(function (response) {
        return response.json()
      })
      .then(function (state) {
        document
          .querySelectorAll('.action-setting')
          .forEach(el => {
            updateValue(el, state[el.id], false)
          })
        hide(waitSettings);
        show(settings);
        show(streamButton);
        startStream();
      })

  // Coloque algún texto útil en el botón 'Fijo'
    stillButton.setAttribute("title", `Capture a still image :: ${baseHost}/capture`);

    const stopStream = () => {
      window.stop();
      streamButton.innerHTML = 'V&iacute;deo';
          streamButton.setAttribute("title", `V&iacute;deo :: ${streamURL}`);
          //streamButton.setAttribute("title", `Start the stream :: ${streamURL}`);
      hide(viewContainer);
    }

    const startStream = () => {
      view.src = streamURL;
      view.scrollIntoView(false);
      streamButton.innerHTML = 'V&iacute;deo parado';
      streamButton.setAttribute("title", `Stop the stream`);
      show(viewContainer);
    }

        /* cambio */

     saveButton.onclick = () => {
    var canvas = document.createElement("canvas");
    canvas.width = view.width;
    canvas.height = view.height;
    document.body.appendChild(canvas);
    var context = canvas.getContext('2d');
    context.drawImage(view,0,0);
    try {
      var dataURL = canvas.toDataURL('image/jpeg');
      saveButton.href = dataURL;
      var d = new Date();
      saveButton.download = d.getFullYear() + ("0"+(d.getMonth()+1)).slice(-2) + ("0" + d.getDate()).slice(-2) + ("0" + d.getHours()).slice(-2) + ("0" + d.getMinutes()).slice(-2) + ("0" + d.getSeconds()).slice(-2) + ".jpg";
    } catch (e) {
      console.error(e);
    }
    canvas.parentNode.removeChild(canvas);
  }

 /* fin cambio */

    

    const applyRotation = () => {
      rot = rotate.value;
      if (rot == -90) {
        viewContainer.style.transform = `rotate(-90deg)  translate(-100%)`;
        closeButton.classList.remove('close-rot-none');
        closeButton.classList.remove('close-rot-right');
        closeButton.classList.add('close-rot-left');
      } else if (rot == 90) {
        viewContainer.style.transform = `rotate(90deg) translate(0, -100%)`;
        closeButton.classList.remove('close-rot-left');
        closeButton.classList.remove('close-rot-none');
        closeButton.classList.add('close-rot-right');
      } else {
        viewContainer.style.transform = `rotate(0deg)`;
        closeButton.classList.remove('close-rot-left');
        closeButton.classList.remove('close-rot-right');
        closeButton.classList.add('close-rot-none');
      }
       console.log('Rotation ' + rot + ' applied');
   }

    // Adjuntar acciones a los controles

    stillButton.onclick = () => {
      stopStream();
      view.src = `${baseHost}/capture?_cb=${Date.now()}`;
      view.scrollIntoView(false);
      show(viewContainer);
    }

    closeButton.onclick = () => {
      stopStream();
      hide(viewContainer);
    }

    streamButton.onclick = () => {
      const streamEnabled = streamButton.innerHTML === 'Stop Stream'
      if (streamEnabled) {
        stopStream();
      } else {
        startStream();
      }
    }

    // Adjuntar valor predeterminado en la acción de cambio
    document
      .querySelectorAll('.action-setting')
      .forEach(el => {
        el.onchange = () => updateConfig(el)
      })

    // Actualice los controles deslizantes de rango a medida que se mueven
    document
      .querySelectorAll('input[type="range"]')
      .forEach(el => {
        el.oninput = () => updateRangeConfig(el)
      })

    // Custom actions
    // Detection and framesize
    rotate.onchange = () => {
      applyRotation();
      updateConfig(rotate);
    }

    framesize.onchange = () => {
      updateConfig(framesize)
    }

    swapButton.onclick = () => {
      window.open('/?view=full','_self');
    }

  })
  </script>
</html>)=====";

size_t index_simple_html_len = sizeof(index_simple_html)-1;

/* Stream Viewer */

const uint8_t streamviewer_html[] = R"=====(<!doctype html>
<html>
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width,initial-scale=1">
    <title id="title">ESP32-CAM StreamViewer</title>
    <link rel="icon" type="image/png" sizes="32x32" href="/favicon-32x32.png">
    <link rel="icon" type="image/png" sizes="16x16" href="/favicon-16x16.png">
    <style>
      /* No stylesheet, define all style elements here */
      body {
        font-family: Arial,Helvetica,sans-serif;
        background: #181818;
        color: #EFEFEF;
        font-size: 16px;
        margin: 0px;
        overflow:hidden;
      }

      img {
        object-fit: contain;
        display: block;
        margin: 0px;
        padding: 0px;
        width: 100vw;
        height: 100vh;
      }

      .loader {
        border: 0.5em solid #f3f3f3;
        border-top: 0.5em solid #000000;
        border-radius: 50%;
        width: 1em;
        height: 1em;
        -webkit-animation: spin 2s linear infinite; /* Safari */
        animation: spin 2s linear infinite;
      }

      @-webkit-keyframes spin {   /* Safari */
        0% { -webkit-transform: rotate(0deg); }
        100% { -webkit-transform: rotate(360deg); }
      }

      @keyframes spin {
        0% { transform: rotate(0deg); }
        100% { transform: rotate(360deg); }
      }
    </style>
  </head>

  <body>
    <section class="main">
      <div id="wait-settings" style="float:left;" class="loader" title="Waiting for stream settings to load"></div>
      <div style="display: none;">

        <!-- Hide the next entries, they are present in the body so that we
             can pass settings to/from them for use in the scripting -->
        <div id="rotate" class="action-setting hidden">0</div>
        <div id="cam_name" class="action-setting hidden"></div>
        <div id="stream_url" class="action-setting hidden"></div>
      </div>
      <img id="stream" src="">
    </section>
  </body>

  <script>
  document.addEventListener('DOMContentLoaded', function (event) {
    var baseHost = document.location.origin;
    var streamURL = 'Undefined';

    const rotate = document.getElementById('rotate')
    const stream = document.getElementById('stream')
    const spinner = document.getElementById('wait-settings')

    const updateValue = (el, value, updateRemote) => {
      updateRemote = updateRemote == null ? true : updateRemote
      let initialValue
      if (el.type === 'checkbox') {
        initialValue = el.checked
        value = !!value
        el.checked = value
      } else {
        initialValue = el.value
        el.value = value
      }

      if (updateRemote && initialValue !== value) {
        updateConfig(el);
      } else if(!updateRemote){
        if(el.id === "cam_name"){
          window.document.title = value;
          stream.setAttribute("title", value + "\n(doubleclick for fullscreen)");
          console.log('Name set to: ' + value);
        } else if(el.id === "rotate"){
          rotate.value = value;
          console.log('Rotate recieved: ' + rotate.value);
        } else if(el.id === "stream_url"){
          streamURL = value;
          console.log('Stream URL set to:' + value);
        }
      }
    }

    // read initial values
    fetch(`${baseHost}/info`)
      .then(function (response) {
        return response.json()
      })
      .then(function (state) {
        document
          .querySelectorAll('.action-setting')
          .forEach(el => {
            updateValue(el, state[el.id], false)
          })
        spinner.style.display = `none`;
        applyRotation();
        startStream();
      })

    const startStream = () => {
      stream.src = streamURL;
      stream.style.display = `block`;
    }

    const applyRotation = () => {
      rot = rotate.value;
      if (rot == -90) {
        stream.style.transform = `rotate(-90deg)`;
      } else if (rot == 90) {
        stream.style.transform = `rotate(90deg)`;
      }
      console.log('Rotation ' + rot + ' applied');
    }

    stream.ondblclick = () => {
      if (stream.requestFullscreen) {
        stream.requestFullscreen();
      } else if (stream.mozRequestFullScreen) { /* Firefox */
        stream.mozRequestFullScreen();
      } else if (stream.webkitRequestFullscreen) { /* Chrome, Safari and Opera */
        stream.webkitRequestFullscreen();
      } else if (stream.msRequestFullscreen) { /* IE/Edge */
        stream.msRequestFullscreen();
      }
    }
  })
  </script>
</html>)=====";

size_t streamviewer_html_len = sizeof(streamviewer_html)-1;

/* Captive Portal page
   we replace the <> delimited strings with correct values as it is served */

const std::string portal_html = R"=====(<!doctype html>
<html>
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width,initial-scale=1">
    <title id="title"><CAMNAME> - portal</title>
    <link rel="icon" type="image/png" sizes="32x32" href="<APPURL>favicon-32x32.png">
    <link rel="icon" type="image/png" sizes="16x16" href="<APPURL>favicon-16x16.png">
    <link rel="stylesheet" type="text/css" href="<APPURL>style.css">
  </head>
  <body style="text-align: center;">
    
    <img src="<APPURL>logo.svg" style="position: relative; float: right;">
    <h1><CAMNAME> - access portal</h1>
    <div class="input-group" style="margin: auto; width: max-content;">
      <a href="<APPURL>?view=simple" title="Click here for a simple view with minimum control" style="text-decoration: none;" target="_blank">
      <button>Simple Viewer</button></a>
      <a href="<APPURL>?view=full" title="Click here for the main camera page with full controls" style="text-decoration: none;" target="_blank">
      <button>Full Viewer</button></a>
      <a href="<STREAMURL>view" title="Click here for the dedicated stream viewer" style="text-decoration: none;" target="_blank">
      <button>Stream Viewer</button></a>
    </div>
    <hr>
    <a href="<APPURL>dump" title="Information dump page" target="_blank">Camera Details</a><br>
  </body>
</html>)=====";

/* Error page
   we replace the <> delimited strings with correct values as it is served */

const std::string error_html = R"=====(<!doctype html>
<html>
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width,initial-scale=1">
    <title id="title"><CAMNAME> - Error</title>
    <link rel="icon" type="image/png" sizes="32x32" href="/favicon-32x32.png">
    <link rel="ico\" type="image/png" sizes="16x16" href="/favicon-16x16.png">
    <link rel="stylesheet" type="text/css" href="<APPURL>style.css">
  </head>
  <body style="text-align: center;">
    <img src="<APPURL>logo.svg" style="position: relative; float: right;">
    <h1><CAMNAME></h1>
    <ERRORTEXT>
  </body>
  <script>
    setTimeout(function(){
      location.replace(document.URL);
    }, 60000);
  </script>
</html>)=====";
