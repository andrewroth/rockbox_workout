$(function() {
  $(".actual").change(function() {
    sle_id = $(this).attr('sle_id');
    console.log('sle_id: ' + sle_id);
    $.ajax({
      url: '/set_log_entries/'+$(this).attr('sle_id'),
      //data: { 'set_log_entry[reps]' : $('actual_reps_'+sle_id).val(), 'set_log_entry[weight]' : $('actual_wt_'+sle_id).val()},
      data: 'set_log_entry[reps]='+$('#actual_reps_'+sle_id).val()+'&set_log_entry[weight]='+$('#actual_wt_'+sle_id).val()+'&format=js',
      type: 'PUT'
    });
  });
});
