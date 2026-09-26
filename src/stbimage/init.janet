(import _stbimage :as core)

(defn load [path &opt req-comp] 
    (core/load path (or req-comp 0)))

(defn loadf [path &opt req-comp] 
    (core/loadf path (or req-comp 0)))

(defn load-mem [data &opt req-comp] 
    (core/load-mem data (or req-comp 0)))

(defn loadf-mem [data &opt req-comp] 
    (core/loadf-mem data (or req-comp 0)))

(defn set-flip-vertically! [flip?]
    (core/set-flip-vertically flip?))

(defn info [path] 
    (core/info path))

(defn write-png [img path &named stride]
    (def {:width w :height h :channels comp :data data} img)
    (core/write-png path w h comp data (or stride (* w comp))))

(defn write-bmp [img path]
    (def {:width w :height h :channels comp :data data} img)
    (core/write-bmp path w h comp data))

(defn write-tga [img path]
    (def {:width w :height h :channels comp :data data} img)
    (core/write-tga path w h comp data))

(defn write-jpg [img path &named quality]
    (def {:width w :height h :channels comp :data data} img)
    (core/write-jpg path w h comp data (or quality 90)))
