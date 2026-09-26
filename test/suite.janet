(import ../src/stbimage :as stbi)

(defn map-rgba [img f]
    (def {:width w :height h :channels ch :data data} img)
    (def n-pixels (* w h))
    (for p 0 n-pixels
        (def idx (* p ch))
        (def pixel (string/bytes (buffer/slice data idx (+ idx ch))))
        (def new-pixel (f ;pixel))
        (for c 0 ch
            (put data (+ idx c) (new-pixel c))))
    img)

(let [img (stbi/load "test/neco.png")]
    (assert (> (length (img :data)) 0))
    (assert (= (img :width) 372))
    (assert (= (img :height) 589))
    
    (map-rgba img (fn [r g b a] [(- 255 r) (- 255 g) (- 255 b) a]))
    (stbi/write-png "out.png" img)
)
