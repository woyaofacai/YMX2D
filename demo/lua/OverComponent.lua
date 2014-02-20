require "ymx"
require "MessageCode"

local background	--背景纹理
local font --打印最后得分的字体

local SCREEN_WIDTH = ymx.getWindowWidth()

OverComponent = {
	id = 2,
	visible = false,
	enabled = false,
}

function OverComponent:LoadContent()
	--加载背景纹理
	background = ymx.loadTexture("gameover.png")
	--创建字体对象
	font = ymx.createFont("result.fnt", self)
end

function OverComponent:OnMessage(msgid, notifyCode, params)
	--如果组件接收到GAME_OVER消息
	if msgid == MessageCode.GAME_OVER then
		--从notifyCode参数读到最终分数
		local score = notifyCode
		--为字体对象设置文本
		font:setText("Final Score: "..score)
		--开启当前组件并设置为可见
		ymx.setEnabled(self, true)
		ymx.setVisible(self, true)
		return true
	end
	return false
end

function OverComponent:Update(delta)
	--如果玩家按下ESC键，则退出程序
	if ymx.keyDown(ymx.Keys.ESCAPE) then
		return false
	end
end

function OverComponent:Render(g)
	--绘制背景纹理
	ymx.renderTexture(g, background, 0, 0)
	--打印最终分数
	font:render(g, (SCREEN_WIDTH - font:getWidth()) * 0.5, 350)
end

function OverComponent:UnloadContent()
	font:release()
	background:release()
end
