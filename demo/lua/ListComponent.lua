require "ymx"

local font
local listView

ListComponent = {
	id = 1
}

function ListComponent:LoadContent()
	font = ymx.createFont("list.fnt")
	--创建列表，该列表的id为1，位置坐标为(120, 80)
	listView = ymx.createListView(1, self, nil, font, 120, 80)
end

function ListComponent:Initialize()
	--将列表的每个列表项宽度设为160,高度设为 35
	listView:setItemSize(160, 35)

	--为列表项的不同状态设置文本颜色
	listView:setTextColor{
		NORMAL = {255, 255, 0},
		MOUSEOVER = {255, 255, 255},
		PRESSED = {255, 0, 0},
	}

	--向列表中添加4个列表项
	listView:addItem("Start Game")
	listView:addItem("Leaderboard")
	listView:addItem("Help")
	listView:addItem("Quit")
	
	--列表项文本居中
	listView:setTextAlignment("center")
end

function ListComponent:Update(delta)
	listView:update(delta)
end

function ListComponent:Render(g)
	listView:render(g)
end

function ListComponent:OnMessage(msgid, notifyCode, params)
	if msgid == ymx.Msg.LIST_CLICK then --如果是单击列表项事件
		if notifyCode == 1 then	--如果列表控件ID为1
			local index = params.index	--读取被点击的列表项索引
			ymx.messageBox("You click the "..index.." item!")
			return true
		end
	end
	return false
end

function ListComponent:UnloadContent()
	listView:release()
	font:release()
end