
function searchUrl()
{
	var msg = document.getElementById("msg");

	var f = document.user_info;
	var q = f.q.value;
	var url = "cgi-bin/sh_cgi/search_url.cgi";
	var postStr   =  "q=" + q;

	var ajax = false;
	if(window.XMLHttpRequest) 
    { 	
        ajax = new XMLHttpRequest();
		if (ajax.overrideMimeType) 
		{
			ajax.overrideMimeType("text/xml");
		}
	}
	else if (window.ActiveXObject) 
    {
		try 
		{
			ajax = new ActiveXObject("Msxml2.XMLHTTP");
		} 
		catch (e) 
		{
			try 
			{
				ajax = new ActiveXObject("Microsoft.XMLHTTP");
			} 
			catch (e) {}
		}
	}
	if (!ajax) 
    {
        alert("Your browser does not support XMLHTTP.");
		return false;
	}

	ajax.open("POST", url, true);
	ajax.setRequestHeader("Content-Type","application/x-www-form-urlencoded");
	ajax.send(postStr);
	ajax.onreadystatechange = function() 
	{ 
		if (ajax.readyState == 4 && ajax.status == 200) 
		{ 
			msg.innerHTML = ajax.responseText; 
		} 
	} 
}

var xmlhttp;
function loadAllObjs()
{
	var url = "cgi-bin/sh_cgi/show_all_urls.cgi";
    xmlhttp=null;
    
    var f = document.show_obj;
	//var num = f.num.value;
    var per_page = f.per_page.value;
    var cur_page = f.cur_page.value;
	var postStr   =  "per_page=" + per_page + "&cur_page=" + cur_page;
    if (window.XMLHttpRequest)
    {// code for Firefox, Opera, IE7, etc.
        xmlhttp=new XMLHttpRequest();
    }
    else if (window.ActiveXObject)
    {// code for IE6, IE5
        xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
    }
    if (xmlhttp!=null)
    {
        xmlhttp.onreadystatechange=state_Change;
        xmlhttp.open("POST",url,true);
        xmlhttp.send(postStr);
    }
    else
    {
        alert("Your browser does not support XMLHTTP.");
    }
}

function state_Change()
{
    if (xmlhttp.readyState==4)
    {// 4 = "loaded"
        if (xmlhttp.status==200)
        {// 200 = "OK"
            document.getElementById('allObjs').innerHTML=xmlhttp.responseText;
        }
        else
        {
            alert("Problem retrieving data:" + xmlhttp.statusText);
        }
    }
}


function addUrl()
{
	var url = "cgi-bin/sh_cgi/add_url.cgi";
    xmlhttp=null;
    
    var f = document.addurl_form;
    var url_tmp = f.url.value;
	var postStr   =  "url=" + url_tmp;
    if (window.XMLHttpRequest)
    {// code for Firefox, Opera, IE7, etc.
        xmlhttp=new XMLHttpRequest();
    }
    else if (window.ActiveXObject)
    {// code for IE6, IE5
        xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
    }
    if (xmlhttp!=null)
    {
        xmlhttp.onreadystatechange=addurl_state_Change;
        xmlhttp.open("POST",url,true);
        xmlhttp.send(postStr);
    }
    else
    {
        alert("Your browser does not support XMLHTTP.");
    }
}

function addurl_state_Change()
{
    if (xmlhttp.readyState==4)
    {// 4 = "loaded"
        if (xmlhttp.status==200)
        {// 200 = "OK"
            document.getElementById('addurl_msg').innerHTML=xmlhttp.responseText;
        }
        else
        {
            alert("Problem retrieving data:" + xmlhttp.statusText);
        }
    }
}

//function getTotalCnt()
//{
//	var url = "cgi-bin/sh_cgi/get_total_cnt.cgi";
//    xmlhttp=null;
//    
//    var f = document.addurl_form;
//    var url_tmp = f.url.value;
//	//var postStr   =  "url=" + url_tmp;
//    if (window.XMLHttpRequest)
//    {// code for Firefox, Opera, IE7, etc.
//        xmlhttp=new XMLHttpRequest();
//    }
//    else if (window.ActiveXObject)
//    {// code for IE6, IE5
//        xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
//    }
//    if (xmlhttp!=null)
//    {
//        xmlhttp.onreadystatechange=totalcnt_state_Change;
//        xmlhttp.open("POST",url,true);
//        xmlhttp.send(null);
//    }
//    else
//    {
//        alert("Your browser does not support XMLHTTP.");
//    }
//}
//
//function totalcnt_state_Change()
//{
//    if (xmlhttp.readyState==4)
//    {// 4 = "loaded"
//        if (xmlhttp.status==200)
//        {// 200 = "OK"
//            document.getElementById('total_objs').innerHTML=xmlhttp.responseText;
//        }
//        else
//        {
//            alert("Problem retrieving data:" + xmlhttp.statusText);
//        }
//    }
//}

function confirmDel()
{ 
    return confirm("Sure to delete?"); 
}

