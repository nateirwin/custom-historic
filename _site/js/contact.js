/**
 * http://ejohn.org/projects/flexible-javascript-events
 */
function addEvent( obj, type, fn ) {
  if (obj.attachEvent) {
    obj['e' + type + fn] = fn;
    obj[type + fn] = function() {
      obj['e' + type + fn](window.event);
    };
    
    obj.attachEvent('on' + type, obj[type + fn]);
  } else {
    obj.addEventListener(type, fn, false);
  }
}
/**
 *
 */
function setDisabled(field) {
  field.parentNode.parentNode.className = 'control-group error';

  addEvent(field, 'focus', function() {
    field.parentNode.parentNode.className = 'control-group';
  });
}

addEvent(window, 'load', function() {
  var email = document.getElementById('Field3'),
      firstName = document.getElementById('Field1'),
      inquiry = document.getElementById('Field4'),
      lastName = document.getElementById('Field2');

  $('#form').submit(function(e) {
    var error = false;

    if (email.value.length === 0) {
      error = true;
      setDisabled(email);
    }

    if (firstName.value.length === 0) {
      error = true;
      setDisabled(firstName);
    }

    if (inquiry.value.length === 0) {
      error = true;
      setDisabled(inquiry);
    }

    if (lastName.value.length === 0) {
      error = true;
      setDisabled(lastName);
    }

    if (error) {
      return false;
    } else {
      document.getElementById('submit').disabled = true;

      /*
      e.preventDefault();
      $.ajax({
        data: $('#form').serialize(),
        success: function() {
          alert('Success!');
        },
        type: 'POST',
        url: 'http://customhistoric.wufoo.com/forms/inquiry/#public'
      });
      */

      return true;
    }
  });
});