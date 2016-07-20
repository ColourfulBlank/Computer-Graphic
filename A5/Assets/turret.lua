
rootnode = gr.node('root')
rootnode:rotate('y', -20.0)
-- rootnode:scale( 0.25, 0.25, 0.25 )
rootnode:translate(0.0, 0.0, -1.0)

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)
yellow = gr.material({1.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
black = gr.material({0.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
skin = gr.material({1.0, 0.8, 0.6}, {0.1, 0.1, 0.1}, 5)
brown = gr.material({0.4, 0.2, 0}, {0.1, 0.1, 0.1}, 5)
lightGreen = gr.material({0, 0.8, 0}, {0.1, 0.1, 0.1}, 5)

ground = gr.mesh('plane', 'ground')
rootnode:add_child(ground)
ground:scale(15,0.1,15);
ground:rotate("x",30);
ground:set_material(red)

-- ball thrower
body = gr.mesh('cube', 'body')
ground:add_child(body)
body:scale(1,0.3,1)
body:translate(0, 0.1, 0)
body:set_material(green)

xz_joint = gr.mesh('sphere', 'xz_joint')
body:add_child(xz_joint)
xz_joint:scale(0.1, 0.1, 0.1)
xz_joint:translate(0.0, 0.2, 0.0)
xz_joint:set_material(blue)

xz_joint_joint = gr.joint('xz_joint_joint', {0, 0, 0}, {-180, 0, 180})
xz_joint:add_child(xz_joint_joint)

xz_rotation_plate = gr.mesh('Cylinder', 'xz_rotation_plate')
xz_rotation_plate:scale( 0.5, 0.2, 0.5 )
xz_joint_joint:add_child(xz_rotation_plate)
xz_rotation_plate:set_material(blue)

gunBodyRight = gr.mesh('cube', 'gunBodyRight')
gunBodyRight:scale( 1, 0.2, 0.2 )
gunBodyRight:translate(0, 0.2, 0.2)
xz_rotation_plate:add_child(gunBodyRight)
gunBodyRight:set_material(red)

gunBodyLeft = gr.mesh('cube', 'gunBodyLeft')
gunBodyLeft:scale( 1, 0.2, 0.2 )
gunBodyLeft:translate(0, 0.2, -0.2)
xz_rotation_plate:add_child(gunBodyLeft)
gunBodyLeft:set_material(red)

y_joint = gr.mesh('sphere', 'y_joint')
xz_rotation_plate:add_child(y_joint)
y_joint:scale(0.1, 0.1, 0.1)
y_joint:translate(0.4, 0.2, 0.0)
y_joint:set_material(yellow)

y_joint_joint = gr.joint('y_joint_joint', {0, 0, 90}, {0, 0, 0})
y_joint:add_child(y_joint_joint)

arm = gr.mesh('Cylinder', 'arm')
arm:scale(0.2, 0.8, 0.2)
arm:translate(0.0, 0.5, 0.0)
arm:set_material(green)
y_joint_joint:add_child(arm)

ball = gr.ball('sphere', 'ball')
ball:scale(0.2, 0.2, 0.2)
ball:translate(0.0, 0.6, 0.0)
ball:set_material(red)
arm:add_child(ball)

explosive = gr.exp('mesh', 'partical')
explosive:scale(0.1, 0.1, 0.1)
explosive:set_material(white)
ball:add_child(explosive)

explosive_1 = gr.exp('mesh', 'partical_1')
explosive_1:scale(0.1, 0.1, 0.1)
explosive_1:set_material(yellow)
ball:add_child(explosive_1)

explosive_2 = gr.exp('mesh', 'partical_2')
explosive_2:scale(0.1, 0.1, 0.1)
explosive_2:set_material(green)
ball:add_child(explosive_2)

explosive_3 = gr.exp('mesh', 'partical_3')
explosive_3:scale(0.1, 0.1, 0.1)
explosive_3:set_material(blue)
ball:add_child(explosive_3)

explosive_4 = gr.exp('mesh', 'partical_4')
explosive_4:scale(0.1, 0.1, 0.1)
explosive_4:set_material(red)
ball:add_child(explosive_4)



return rootnode
