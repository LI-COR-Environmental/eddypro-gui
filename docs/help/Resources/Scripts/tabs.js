/*begin selectable tabs*/
function show(evt, products) 
{
var i, tabcontent, tablinks;
tabcontent = document.getElementsByClassName("tabcontent");
for (i = 0; i < tabcontent.length; i++) 
	{
	tabcontent[i].style.display = "none";
	}
	tablinks = document.getElementsByClassName("tablinks");
	for (i = 0; i < tablinks.length; i++) 
	{
	tablinks[i].className = tablinks[i].className.replace(" active", "");
	}
	document.getElementById(products).style.display = "block";
	evt.currentTarget.className += " active";
	}
/*end selectable tabs*/
/*begin sortable table*/
function sortTable() {
	var input, filter, table, tr, td, i;
	input = document.getElementById("sortInput");
	filter = input.value.toUpperCase();
	table = document.getElementById("docsTable");
	tr = table.getElementsByTagName("tr");
	for (i = 0; i < tr.length; i++) {
		td = tr[i].getElementsByTagName("td")[0];
		if (td) {
			if (td.innerHTML.toUpperCase().indexOf(filter) > -1) {
				tr[i].style.display = "";
			} else {
				tr[i].style.display = "none";
			}
		}       
	}
}
/*end sortable table*/
/*begin filter table*/
function filterTable() {
	var input, filter, table, tr, td, i;
	input = document.getElementById("filterInput");
	filter = input.value.toUpperCase();
	table = document.getElementById("docsTable");
	tr = table.getElementsByTagName("tr");
	for (i = 0; i < tr.length; i++) {
		td = tr[i].getElementsByTagName("td")[2];
		if (td) {
			if (td.innerHTML.toUpperCase().indexOf(filter) > -1) {
				tr[i].style.display = "";
			} else {
				tr[i].style.display = "none";
			}
		}       
	}
}
/*end filter table*/
/*begin feedback fade in*/
$(document).ready(function() {
		$(".survey").delay(3000).fadeIn(500);
	});
/*end feedback fade in*/

$(document).ready(function(){
   
		/* -------------- Feedback Thanks to David Lee on the MadCap Flare Forum for this useful trick-------------- */
		/* Yes click */
		$('.feedback-yes').click(function() {
				/* fade out question, fade in thankyou message */
				$('.feedback-question').fadeOut(function() {
						$('.feedback-reason.yes-thanks').fadeIn();
					});
			});
		/* No click */
		$('.feedback-no').click(function() {
				/* fade out question, fade in thankyou message */
				$('.feedback-question').fadeOut(function() {
						$('.feedback-reason.no').fadeIn();
					});
			});
		/* No - response reasons */
		$('.feedback-reason .option').change(function() {
				/* if any options change, disable checkbox */
				$(this).attr('disabled', true);
			});
	});   
/*end feedback form*/
/*launches email with "to" "subject" and "body" populated.*/
function emailCurrentPage(){
	window.location.href = "mailto:envfeedback@licor.com?subject=" + document.title + "&body=" + escape(window.location.href);
}
/*end email customizations*/
/*begin clear search filter and reload page*/
	function clearFilter() {
		var newURL = location.href.split("&")[0]; window.history.pushState('object', document.title, newURL);
		location.reload();}
/*end clear search filter and reload page*/

/*begin sticky-nav scrolling*/
$(window).scroll(function() {
		if ($(this).scrollTop() > 320){
			$('div.in-page-sticky-mid').removeClass("in-page-sticky-mid").addClass("in-page-sticky-top");
		} 
		else {
			$('div.in-page-sticky-top').removeClass("in-page-sticky-top").addClass("in-page-sticky-mid");
		}
	});
/*end sticky-nav scrolling*/
