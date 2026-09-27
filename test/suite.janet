(import ../src/stbimage :as stbi)

(defn map-rgba [img f]
    (let [{:width w :height h :channels ch :data data} img
          n-pixels (* w h)]
        (assert (= ch 4) "Image must have 4 channels")
        
        (for p 0 n-pixels
            (let [idx (* p ch)
                  r (data idx)
                  g (data (+ idx 1))
                  b (data (+ idx 2))
                  a (data (+ idx 3))
                  [nr ng nb na] (f r g b a)]
                (put data idx nr)
                (put data (+ idx 1) ng)
                (put data (+ idx 2) nb)
                (put data (+ idx 3) na)))
        img))

(defmacro foreach-pixel [img x-sym y-sym idx-sym & body]
    ~(let [w (,img :width)
           h (,img :height)
           ch (,img :channels)]
        (for ,y-sym 0 h
            (for ,x-sym 0 w
                (let [,idx-sym (* (+ (* ,y-sym w) ,x-sym) ch)]
                ,;body)))))

(defn apply-bayer-dithering [img]
    (def data (img :data))
    (def bayer [0 8 2 10 12 4 14 6 3 11 1 9 15 7 13 5])
    (def color-chans (if (<= (img :channels) 2) 1 3))

    (foreach-pixel img x y base-idx
        (let [bval (bayer (+ (% x 4) (* (% y 4) 4)))
              threshold (* bval 16)]
            (for c 0 color-chans
                (let [idx (+ base-idx c)
                      v (data idx)]
                    (put data idx (if (> v threshold) 255 0))))))
    img)

(let [img (stbi/load "test/neco.png")]
    (assert (> (length (img :data)) 0))
    (assert (= (img :width) 372))
    (assert (= (img :height) 589))
    (map-rgba img (fn [r g b a] [(- 255 r) (- 255 g) (- 255 b) a]))
    (apply-bayer-dithering img)
    (stbi/write-png img "out.png")
)
