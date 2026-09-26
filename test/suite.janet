(import ../src/stbimage :as stbi)

(let [img (stbi/loadf "test/neco.png")]
    (assert (> (length (img :data)) 0))
    (assert (= (img :width) 372))
    (assert (= (img :height) 589)))
