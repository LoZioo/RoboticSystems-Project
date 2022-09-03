const PACKET_ARGV_MAXLEN = 4;

$(document).ready(function(){
	//Soft reset button.
	$("#soft_reset").click(() => {
		$.post("/settings/reset");
		location.reload();
	});

	//OTA button.
	$("#ota_update").click(() => location.href = "/update");

	//Settings.bin button.
	$("#settings_download").click(() => window.open("/settings.bin"));

	//Reset buttons.
	$("#configs_reset").click(() => $(".configs .val input").val("").attr("placeholder", ""));
	$("#wifi_reset").click(() => $(".wifi_data .val input").val(""));

	//Auto command submit on enter keypress.
	$(".configs .val input").keypress(e => {
		if(e.key == "Enter")
			$("#configs_submit").trigger("click");
	});

	//Send command.
	$("#configs_submit").click(async () => {
		//Retrive command number.
		const com = parseInt($("#configs_commands").val());
		
		//Retrive args.
		let args = [];
		for(let i=0; i<PACKET_ARGV_MAXLEN; i++){
			const val = $("#arg_" + i).val();
			args.push(val == "" ? 0 : parseFloat(val));
		}
		
		if(args.includes(NaN)){
			alert("Errore: uno degli argomenti contiene un valore float non valido.");
			return;
		}

		//Pack data.
		const data = { com, args }

		let res;
		try{
			res = await $.post("/commands/general", { data: JSON.stringify(data) });
			// res = { com: 10, args: [5, 6, 7, 8] };
		}

		catch(e){
			ajax_error(e);
			return;
		}

		//Print command results as placeholders.
		res.args.forEach((e, i) => $("#arg_" + i).val("").attr("placeholder", e));
	});

	//Clear placeholders on command change.
	$("#configs_commands").change(() => {
		for(let i=0; i<PACKET_ARGV_MAXLEN; i++)
			$("#arg_" + i).val("").attr("placeholder", "");
	});

	//Fixed graph.
	$("#add_obstacle_submit, #remove_obstacle_submit, #exec_route_submit").click(async function(){
		//Identify pressed button.
		let url;

		switch($(this).attr("id")){
			case "add_obstacle_submit":
				url = "/commands/obstacle/add";
				break;
			
			case "remove_obstacle_submit":
				url = "/commands/obstacle/remove";
				break;

			case "exec_route_submit":
				url = "/commands/exec_route";
				break;
		}

		//Retrive x and y.
		const x = parseFloat($("#x_val").val());
		const y = parseFloat($("#y_val").val());

		//Clear textbox.
		$("#x_val, #y_val").val("");

		if(isNaN(x) || isNaN(y)){
			alert("Errore: una delle due coordinate e' un valore float non valido.");
			return;
		}

		try{
			await $.post(url, { x, y })
		}

		catch(e){
			ajax_error(e);
			return;
		}
	});

	//Auto WiFi submit on enter keypress.
	$(".wifi_data .val input").keypress(e => {
		if(e.key == "Enter")
			$("#wifi_submit").trigger("click");
	});

	//Set WiFi.
	$("#wifi_submit").click(() => {
		const ssid = $("#wifi_ssid").val();
		const pass = $("#wifi_pass").val();
		
		if(ssid == ""){
			alert("Inserisci un SSID valido!");
			$("#wifi_ssid").focus();
			return;
		}
		
		if(pass == ""){
			alert("Inserisci una password valida!");
			$("#wifi_pass").focus();
			return;
		}

		$.post("/settings/wifi", { ssid, pass });
		
		alert("Connessione in corso; il dispositivo verra' riavviato.");
		location.reload();
	});

	setup();
});

async function setup(){
	//SSID placeholder refresh.
	let ssid;
	try{
		ssid = await $.get("/settings/ssid");
	}

	catch(e){
		ajax_error(e);
		return;
	}

	$("#wifi_ssid").attr("placeholder", ssid);
}

//Try-Catch error printer.
function ajax_error(e){
	console.error("Errore " + e.status + ": " + e.statusText + "; ricaricare la pagina.");
}
