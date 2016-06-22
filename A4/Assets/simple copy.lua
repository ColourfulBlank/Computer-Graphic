-- A simple scene with some miscellaneous geometry.

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)
mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25)

scene_root = gr.node('root')


-- A small stellated dodecahedron.

-- cow = gr.mesh( 'dodec', 'Assets/cow.obj' )
-- cow:set_material(mat3)
-- scene_root:add_child(cow)
s1 = gr.nh_sphere('s1', {0, 200, 0}, 100)
scene_root:add_child(s1)
s1:set_material(mat1)

s6 = gr.nh_sphere('s6', {0, -150, 0}, 100)
scene_root:add_child(s6)
s6:set_material(mat2)

s2 = gr.nh_sphere('s2', {200, 30, -100}, 150)
scene_root:add_child(s2)
s2:set_material(mat1)

s7 = gr.nh_sphere('s7', {-200, 30, -100}, 150)
scene_root:add_child(s7)
s7:set_material(mat1)

-- s3 = gr.nh_sphere('s3', {0, -1200, -500}, 1000)
-- scene_root:add_child(s3)
-- s3:set_material(mat2)


-- b1 = gr.nh_box('b1', {-200, -125, 0}, 100)
-- scene_root:add_child(b1)
-- b1:set_material(mat4)

s4 = gr.nh_sphere('s4', {0, 25, 0}, 50)
scene_root:add_child(s4)
s4:set_material(mat3)

-- s5 = gr.nh_sphere('s5', {0, 100, -250}, 25)
-- scene_root:add_child(s5)
-- s5:set_material(mat1)

-- s6 = gr.nh_sphere('s6', {0, 0, -200}, 140)
-- scene_root:add_child(s6)
-- s6:set_material(mat1)






-- steldodec = gr.mesh( 'dodec', 'Assets/smstdodeca.obj' )
-- steldodec:set_material(mat3)
-- scene_root:add_child(steldodec)

-- white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
white_light = gr.light({0.0, 0.0, 4000.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
-- orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene_root, 'my.png', 640, 480,
	-- gr.render(scene_root, 'my.png', 256, 256,
	  {0, 0, 800}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})
