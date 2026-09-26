(import ../src/stbimage :as stbi)

(defn map-rgba [img f]
    (def {:width w :height h :channels ch :data data} img)
    (assert (= ch 4))
    (def n-pixels (* w h))
    (for p 0 n-pixels
        (def idx (* p ch))
        (def pixel (string/bytes (buffer/slice data idx (+ idx ch))))
        (def new-pixel (f ;pixel))
        (for c 0 ch
            (put data (+ idx c) (new-pixel c))))
    img)

(defn apply-bayer-dithering [img]
    (def {:width w :height h :channels ch :data data} img)
    
    (def bayer-matrix 
        (tuple  0   8   2   10
                12  4   14  6
                3   11  1   9
                15  7   13  5))
    
    (def color-chans (if (<= ch 2) 1 3)) 
    
    (for y 0 h
        (for x 0 w
            (def bx (% x 4))
            (def by (% y 4))
            (def bval (in bayer-matrix (+ bx (* by 4))))
            
            (def threshold (* bval 16))
            
            (def base-idx (* (+ (* y w) x) ch))
            
            (for c 0 color-chans
                (def idx (+ base-idx c))
                (def v (data idx))
                
                (put data idx (if (> v threshold) 255 0)))))
            
    img)

(let [img (stbi/load "test/neco.png")]
    (assert (> (length (img :data)) 0))
    (assert (= (img :width) 372))
    (assert (= (img :height) 589))
    (map-rgba img (fn [r g b a] [(- 255 r) (- 255 g) (- 255 b) a]))
    (apply-bayer-dithering img)
    (stbi/write-png img "out.png")
)
