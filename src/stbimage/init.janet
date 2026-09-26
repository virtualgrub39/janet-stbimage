(import _stbimage :as core)

(defn load [path &opt req-comp] 
    (core/load path (or req-comp 0)))

(defn loadf [path &opt req-comp] 
    (core/loadf path (or req-comp 0)))

(defn info [path] 
    (core/info path))

(defn write-png [path img &named stride]
    (def {:width w :height h :channels comp :data data} img)
    (core/write-png path w h comp data (or stride (* w comp))))

(defn write-bmp [path img]
    (def {:width w :height h :channels comp :data data} img)
    (core/write-bmp path w h comp data))

(defn write-tga [path img]
    (def {:width w :height h :channels comp :data data} img)
    (core/write-tga path w h comp data))

(defn write-jpg [path img &named quality]
    (def {:width w :height h :channels comp :data data} img)
    (core/write-jpg path w h comp data (or quality 90)))
