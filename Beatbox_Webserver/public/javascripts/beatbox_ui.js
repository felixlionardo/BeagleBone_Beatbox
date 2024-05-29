var socket = io.connect();
var replyTimeoutDuration;

function showErrorBox() {
    replyTimeoutDuration = setTimeout(function() {
        $('#error-box').show();
    }, 100);
}



$(document).ready(function() {
    $('#modeNone').click(function() {
        socket.emit('message', 'mode_none');
    });
    $('#modeRock1').click(function() {
        socket.emit('message', 'mode_one');
    });
    $('#modeRock2').click(function() {
        socket.emit('message', 'mode_two');
    });
    $('#volumeDown').click(function() {
        socket.emit('message', 'volume_down');
    });
    $('#volumeUp').click(function() {
        socket.emit('message', 'volume_up');
    });
    $('#bpmDown').click(function() {
        socket.emit('message', 'bpm_down');
    });
    $('#bpmUp').click(function() {
        socket.emit('message', 'bpm_up');
    });
    $('#hiHatSound').click(function() {
        socket.emit('message', 'hi_hat');
    });
    $('#snareSound').click(function() {
        socket.emit('message', 'snare');
    });
    $('#bassSound').click(function() {
        socket.emit('message', 'bass');
    });
    $('#stop').click(function() {
        socket.emit('message', 'stop');
    });

    setInterval(function() {
        socket.emit('message', 'info');
        showErrorBox();
    }, 100);


    socket.on('commandReply', function(command) {
        console.log('Received command: ' + command);
        $('#error-box').hide();
        clearTimeout(replyTimeoutDuration);

        var bpm = command.match(/BPM:(\d+)/)[1];
        var volume = command.match(/Volume:(\d+)/)[1];
        var pattern = command.match(/Pattern:(\d+)/)[1];
        var uptime = command.match(/Hours: (\d+) Minutes: (\d+) Seconds: (\d+)/);
        
        bpmid.value = bpm;
        volumeid.value = volume;
        $('#modeid').text(pattern);
        if (pattern == 0) {
            $('#modeid').text("None");
        }
        else if (pattern == 1) {
            $('#modeid').text("Rock 1");
        }
        else if (pattern == 2) {
            $('#modeid').text("Rock 2");
        }

        $('#status').text(uptime[1] + ":" + uptime[2] + ":" + uptime[3] + " (H:M:S)");
    });
});


