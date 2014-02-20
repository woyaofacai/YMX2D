require "math"
require "os"
require "ymx"
require "Moveable"
require "Actor"
require "MessageCode"

--随机数种子
math.randomseed(os.time())

local groundTexture, bulletTexture, killerTexture, girlTexture
local girl
local killers = {} --游戏中的所有杀手
local bullets = {} --游戏中的所有子弹
local life = 5 --生命数
local score = 0 --当前玩家获得的分数
local font --用来打印当前分数和生命的字体

--游戏窗口的宽度和高度
local SCREEN_WIDTH = ymx.getWindowWidth()
local SCREEN_HEIGHT = ymx.getWindowHeight()

MainComponent = {
	id = 1,
	visible = false,
	enabled = false,
}

function MainComponent:LoadContent() 
	--加载纹理
	groundTexture = ymx.loadTexture("ground.png")
	bulletTexture = ymx.loadTexture("bullet.png")
	girlTexture = ymx.loadTexture("girl.png")
	killerTexture = ymx.loadTexture("killer.png")

	--创建小萝莉对象
	girl = Actor:new{
		component = self,
		texture = girlTexture,
		direction = Moveable.RIGHT,
		velocity = {x = 100, y = 100},
	}

	--创建字体
	font = ymx.createFont("score.fnt", self)
end

--根据当前分数和所剩生命，为字体对象设置文本
local function setScoreAndLifeFontText()
	font:setText("Score:"..score.." Life:"..life);
end


function MainComponent:Initialize()
	--将小萝莉初始位置设为窗口中央
	girl.x = (SCREEN_WIDTH - girl.width) * 0.5
	girl.y = (SCREEN_HEIGHT - girl.height) * 0.5
	--初始化字体对象文本
	setScoreAndLifeFontText()
end

--在每帧中更新小萝莉状态
local function updateGirlState(delta)
	if ymx.getKeyState(ymx.Keys.LEFT) then
		if girl.direction ~= Moveable.LEFT then
			girl:setDirection(Moveable.LEFT)
		else 
			girl:move(delta)
			if girl.x < 0 then girl.x = 0 end
		end
	elseif ymx.getKeyState(ymx.Keys.RIGHT) then
		if girl.direction ~= Moveable.RIGHT then
			girl:setDirection(Moveable.RIGHT)
		else 
			girl:move(delta)
			if girl.x + girl.width > SCREEN_WIDTH then 
				girl.x = SCREEN_WIDTH - girl.width 
			end
		end
	elseif ymx.getKeyState(ymx.Keys.UP) then
		if girl.direction ~= Moveable.UP then
			girl:setDirection(Moveable.UP)
		else 
			girl:move(delta)
			if girl.y < 0 then girl.y = 0 end
		end
	elseif ymx.getKeyState(ymx.Keys.DOWN) then
		if girl.direction ~= Moveable.DOWN then
			girl:setDirection(Moveable.DOWN)
		else 
			girl:move(delta)
			if girl.y + girl.height > SCREEN_HEIGHT 
				then girl.y = SCREEN_HEIGHT - girl.height 
			end
		end
	end	
end

--向游戏中添加杀手
local function addKiller()
	--随机取杀手的前进方向
	local direction = math.random(0, 3)
	--将杀手移动速度设为20-200之间的某个随机数
	local speed = math.random() * (100 - 20) + 20
	--创建杀手对象
	local killer = Actor:new{
		direction = direction,
		component = self,
		texture = killerTexture,
		velocity = {x = speed, y = speed}
	}

	--根据前进方向，随机设置杀手的初始位置
	if direction == Moveable.UP then
		killer.y = SCREEN_HEIGHT
		killer.x = math.random() * (SCREEN_WIDTH - killer.width)
	elseif direction == Moveable.DOWN then
		killer.y = -killer.height
		killer.x = math.random() * (SCREEN_WIDTH - killer.width)
	elseif direction == Moveable.LEFT then
		killer.x = SCREEN_WIDTH
		killer.y = math.random() * (SCREEN_HEIGHT - killer.height)
	else 
		killer.x = -killer.width
		killer.y = math.random() * (SCREEN_HEIGHT - killer.height)
	end
	
	--将新创建的杀手添加到killers数组的尾端
	killers[#killers + 1] = killer
end

--判断某个Moveable(或Actor)对象是否跑出游戏窗口
local function isSpriteOutOfScreen(o)
	if o.direction == Moveable.LEFT then return o.x < -o.width end
	if o.direction == Moveable.RIGHT then return o.x > SCREEN_WIDTH end
	if o.direction == Moveable.UP then return o.y < -o.height end
	if o.direction == Moveable.DOWN then return o.y > SCREEN_HEIGHT end
	return false
end

--负责在每帧中更新游戏中的所有杀手的状态
local function updateKillersState(delta)
	local i = 1
	--遍历游戏中的所有杀手
	while i <= #killers do
		local killer = killers[i]
		if isSpriteOutOfScreen(killer) then --如果杀手跑出窗口，则移除它
			killer:release()
			table.remove(killers, i)
		else 
			if killer:isCollide(girl) then --如果该杀手碰到了小萝莉
				life = life - 1
				setScoreAndLifeFontText()
				killer:release()
				table.remove(killers, i)
			else
				--如果该杀手在窗口内，并且没碰到小萝莉，则移动它 
				killer:move(delta)
				i = i + 1
			end
		end
	end
end

--负责玩家按下空格键时，发射出一颗子弹
local function addBullet()
	--创建子弹对象
	local bullet = Moveable:new{
		component = self,
		texture = bulletTexture,
		velocity = {x = 200, y = 200},
		direction = girl.direction,
	}
	--将子弹位置设置为小萝莉的当前位置
	bullet.x = girl.x + (girl.width - bullet.width) * 0.5
	bullet.y = girl.y + (girl.height - bullet.height) * 0.5
	--将新创建的子弹对象添加到bullets对象的尾端
	bullets[#bullets + 1] = bullet
end

--负责在每一帧中更新所有子弹的状态
local function updateBulletsState(delta)
	local i = 1
	--遍历游戏中的所有子弹
	while i <= #bullets do
		local bullet = bullets[i]
		--如果子弹跑出窗口，则移除它
		if isSpriteOutOfScreen(bullet) then
			bullet:release()
			table.remove(bullets, i)
		else
			--测试子弹是否击中某个杀手
			for k, killer in pairs(killers) do
				
				--如果子弹击中该杀手，则移除杀手，并移除子弹
				if killer:isCollide(bullet) then
					killer:release()
					table.remove(killers, k)

					bullet:release()
					bullet = nil
					table.remove(bullets, i)

					score = score + 10
					setScoreAndLifeFontText()

					break
				end
			end
			--如果子弹没击中任何杀手，则移动子弹
			if bullet then
				bullet:move(delta)
				i = i + 1
			end
		end
	end
end


function MainComponent:Update(delta)
	--如果生命数小于等于0，则向OverComponent组件发送GAME_OVER消息
	if life <= 0 then
		ymx.setEnabled(self, false)
		ymx.setEnabled(self, false)
		ymx.sendMessage(MessageCode.GAME_OVER, "Over", score)
		return true
	end

	updateBulletsState(delta)
	updateKillersState(delta)
	
	--按照一定的概率，向游戏中添加杀手
	if math.random(1, 20) == 1 then
		addKiller()
	end

	updateGirlState(delta)

	--如果玩家按下空格键，则发射一枚子弹
	if ymx.keyDown(ymx.Keys.SPACE) then
		addBullet()
	end
end


function MainComponent:Render(g)
	--绘制背景纹理
	ymx.renderTexture(g, groundTexture, 0, 0)
	--绘制小萝莉
	girl:render(g)

	--绘制全部杀手
	for k, v in pairs(killers) do
		v:render(g)
	end
	--绘制全部子弹
	for k,v in pairs(bullets) do
		v:render(g)
	end
	--打印当前分数和所剩生命数
	font:render(g, 0, 0)
end

function MainComponent:UnloadContent()
	for k, v in pairs(killers) do
		v:release()
	end

	for k, v in pairs(bullets) do
		v:release()
	end

	girl:release()

	groundTexture:release()
	bulletTexture:release()
	girlTexture:release()
	killerTexture:release()

	font:release()
end
