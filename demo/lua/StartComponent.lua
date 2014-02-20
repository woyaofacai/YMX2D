require "ymx"

local background	--背景纹理

StartComponent = {
	id = 3,
}


function StartComponent:LoadContent()
	--加载背景纹理
	background = ymx.loadTexture("start.png")
end

function StartComponent:Render(g)
	--绘制背景纹理
	ymx.renderTexture(g, background, 0, 0)
end

function StartComponent:Update(delta)
	--如果玩家按下键盘上的任意键
	if ymx.getInputKey() ~= 0 then
		--禁用并使当前组件不可见
		ymx.setEnabled(self, false)
		ymx.setVisible(self, false)
		--启用MainComponent组件，并使其可见
		ymx.setEnabled("Main", true)
		ymx.setVisible("Main", true)
	end
end

function StartComponent:UnloadContent()
	background:release()
end