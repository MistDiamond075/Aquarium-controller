let allprofile=false;
let allcolor=false;
let selectorprevcolor='red';
let selectorprevprofile='profile1';

const form_profile_set=document.getElementById('set_profile_form');
const form_wifi_set=document.getElementById('set_wifi_form');
const form_time_set=document.getElementById('set_time_form');
const button_server_info=document.getElementById('sv_info_btn');
let form_profile_set_show_selector=document.getElementById('set_profile_form_show_color_all');
let form_profile_set_show_profile_selector=document.getElementById('set_profile_form_show_profile_all');
form_profile_set_show_selector.addEventListener('change',changeCssSelectorFormProfileSet);
form_profile_set_show_profile_selector.addEventListener('change',changeCssSelectorProfileFormProfileSet);

function changeCssSelectorFormProfileSet(){
	if(document.getElementById('profile_form_color_selector').style['pointer-events']==='all'){
		document.getElementById('profile_form_color_selector').style['pointer-events']='none';
		selectorprevcolor=document.getElementById('profile_form_color_selector').value;
		document.getElementById('profile_form_color_selector').value='';
		document.getElementById('profile_form_timer_input').style['display']='none';
		document.getElementById('profile_form_timer_input').value='';
		allcolor=true;
	}else{
		document.getElementById('profile_form_color_selector').style['pointer-events']='all';
		document.getElementById('profile_form_color_selector').value=selectorprevcolor;
		if(document.getElementById('profile_form_profile_selector').style['pointer-events']==='all'){
			document.getElementById('profile_form_timer_input').style['display']='block';
		}
		allcolor=false;
	}
}

function changeCssSelectorProfileFormProfileSet(){
	if(document.getElementById('profile_form_profile_selector').style['pointer-events']==='all'){
		document.getElementById('profile_form_profile_selector').style['pointer-events']='none';
		selectorprevprofile=document.getElementById('profile_form_profile_selector').value;
		document.getElementById('profile_form_profile_selector').value='';
		document.getElementById('profile_form_timer_input').style['display']='none';
		document.getElementById('profile_form_timer_input').value='';
		allprofile=true;
	}else{
		document.getElementById('profile_form_profile_selector').style['pointer-events']='all';
		document.getElementById('profile_form_profile_selector').value=selectorprevprofile;
		if(document.getElementById('profile_form_color_selector').style['pointer-events']==='all'){
			document.getElementById('profile_form_timer_input').style['display']='block';
		}
		allprofile=false;
	}
}

function getServerInfo(){
	//event.preventDefault();
	console.log('send request');
	fetch('/getServerInfo',{
		headers: {
			"Authorization": "Bearer " + localStorage.getItem("jwt")
		}
	})
	.then(response => {
		if(!response.ok){
			document.getElementById('sv_info_content').innerHTML="Error was occurred";
			return null;
		}else{
			console.log(response);
			return response.text();
		}
	})
	.then(data => {
		document.getElementById('sv_info_content').innerHTML =data;
	});
}

function updTime(){
	let newtime=document.getElementById('time_form_input').value;
	if(newtime==='' || newtime===null){
		return;
	}
	const hour=newtime.toString().substring(0,newtime.toString().indexOf(':'));
	const minute=newtime.toString().substring(newtime.toString().indexOf(':')+1);
	const second='00';
	newtime+=":00";
	fetch('/updTime?'+new URLSearchParams({hours:hour,minutes:minute,seconds:second}),{
		method: 'post',
		headers: {
			"Authorization": "Bearer " + localStorage.getItem("jwt")
		}
	}).then(response => {
		if(!response.ok){
			document.getElementById('sv_info_content').innerText ='Error was occurred. Time will not be updated';
		}else{
			document.getElementById('sv_info_content').innerText ='Time updated succesfully';
		}
		console.log(response);
	});
}

function updWifi(){
	let wfssid=document.getElementById('wifi_form_ssid_input').value;
	let wfpassword=document.getElementById('wifi_form_password_input').value;
	if(wfssid==='' && wfpassword===''){
		return;
	}
	const senddata={ssid:wfssid.toString(),password:wfpassword.toString()};
	console.log(senddata);
	const encoded_data=msgpack.encode(senddata);
	fetch('/updWifi',{
		method: 'post',
		headers:{'Content-Type':'application/msgpack',"Authorization": "Bearer " + localStorage.getItem("jwt")},
		body: encoded_data
	}).then(response => {
		if(!response.ok){
			document.getElementById('sv_info_content').innerText ='Error was occurred. Wifi will not be updated';
		}else{
			document.getElementById('sv_info_content').innerText ='Wifi updated succesfully';
		}
		console.log(response);
	});
}

function updProfile(){
	let bright=document.getElementById('profile_form_bright_input').value;
	let timer=document.getElementById('profile_form_timer_input').value;
	if(bright==='' && timer===''){
		return;
	}
	if(bright===''){
		bright=' ';
	}else if(timer===''){
		timer=' ';
	}
	let profile=document.getElementById('profile_form_profile_selector').value;
	let color=document.getElementById('profile_form_color_selector').value;
	if(allprofile){
		profile="all";
	}
	if(allcolor){
		color="all";
	}
	fetch('/updProfile?'+new URLSearchParams({bright:bright,timer:timer,color:color,profile:profile}),{
		method: 'post',
		headers: {
			"Authorization": "Bearer " + localStorage.getItem("jwt")
		}
	}).then(response => {
		if(!response.ok){
			document.getElementById('sv_info_content').innerText ='Error was occurred. Profile will not be updated';
		}else{
			document.getElementById('sv_info_content').innerText ='Profile updated succesfully';
		}
		console.log(response);
	});
}

function updUser(){
	const pw=document.getElementById('user_form_password_input').value;
	const name=document.getElementById('user_form_name_input').value;
	if(pw===undefined || name===undefined || pw==='' || name===''){
		return;
	}
	const senddata={username:name.toString(),password:pw.toString()};
	const encoded_data=msgpack.encode(senddata);
	//console.log(encoded_data);
	fetch('/updUser',{
		method: 'post',
		headers: {
        "Authorization": "Bearer " + localStorage.getItem("jwt")
		},
		body: encoded_data
	}).then(response => {
		if(!response.ok){
			document.getElementById('sv_info_content').innerText ='Error was occurred. Profile will not be updated';
		}else{
			document.getElementById('sv_info_content').innerText ='User data updated succesfully';
		}
		console.log(response);
	});
}

function Logout(){
	fetch('/logout',{
		method:'get',
		headers: {
        "Authorization": "Bearer " + localStorage.getItem("jwt")
		}
	}).then(response =>{
		if(!response.ok){
			document.getElementById('sv_info_content').innerText ='Error was occurred';
		}else{
			localStorage.removeItem("jwt");
			window.location='/loginpage';
		}
	});
}

document.addEventListener('DOMContentLoaded',function(){
	const originalFetch = window.fetch;
	window.fetch = function(url, options = {}) {
	options.headers = options.headers || {};
	const token = localStorage.getItem("jwt");
	if (token) {
		options.headers["Authorization"] = "Bearer " + token;
	}
	return originalFetch(url, options);
	};	
},false)