function showPassword(){
	console.log('1111111');
	const element=document.getElementById('password');
	if(element.getAttribute('type')=='password'){
		element.setAttribute('type','text');
	}else{
		element.setAttribute('type','password');
	}
}

function Login(){
	const pw=document.getElementById('password').value;
	const name=document.getElementById('login').value;
	if(pw===undefined || name===undefined || pw==='' || name===''){
		return;
	}
	const senddata={username:name.toString(),password:pw.toString()};
	const encoded_data=msgpack.encode(senddata);
	//console.log(encoded_data);
	fetch('/login',{
		method: 'post',
		body: encoded_data
	}).then(res => {
	console.log(res);
		return res.text();
	}
	).then(token => {
		localStorage.setItem("jwt", token);
		console.log("JWT сохранён:", localStorage.getItem("jwt"));
	});
	/*const originalFetch = window.fetch;
	window.fetch = function(url, options = {}) {
		options.headers = options.headers || {};
		const token = localStorage.getItem("jwt");
		if (token) {
			options.headers["Authorization"] = "Bearer " + token;
		}
		return originalFetch(url, options);
	};	*/
	fetch("/", {
		method: "GET",
		headers: {
			"Authorization": "Bearer " + localStorage.getItem("jwt")
		}
		}).then(res => res.text())
			.then(html => document.write(html))
			.catch(err => console.error("Ошибка при загрузке страницы:", err));
}

	//headers:{'Content-Type':'application/msgpack'},