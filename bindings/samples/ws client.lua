require "soup-apigen"

soup.tryCatch(function()
	local sched = soup.Scheduler.new()
	local ws_establish_task = soup.EstablishWebSocketConnectionTask.new("wss://ws.postman-echo.com/raw")
	sched:add(ws_establish_task)
	while not ws_establish_task:isWorkDone() do
		sched:tick()
	end
	local sock = ws_establish_task:getSocket()
	local recv_promise = sock:wsRecv()
	sock:wsSend("Hello")
	while recv_promise:isPending() and sched:shouldKeepRunning() do
		sched:tick()
	end
	if not recv_promise:isPending() then
		print("Received: " .. recv_promise:getData())
	else
		print("Error")
	end
end)
