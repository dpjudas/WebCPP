
// We have to do this the old-fashioned way because val::global("JSCallback").new_() doesn't like Javascript classes

function JSCallback(id) {
    this.id = id;
    this.args = [];
    this.retval = undefined;

    this.getHandler = function () {
        return this.handler.bind(this);
    };

    this.getArgs = function () {
        return this.args;
    };

    this.setResult = function (value) {
        this.retval = value;
    };

    this.release = function () {
        this.id = null;
    };

    this.handler = function () {
        if (this.id != null) {
            this.args = Array.from(arguments);
            _invoke_js_callback(this.id);
            var result = this.retval;
            this.args = [];
            this.retval = undefined;
            return result;
        }
    };
}

OAuth = function () {
    function parseJwt(token) {
        var base64Url = token.split('.')[1];
        var base64 = base64Url.replace(/-/g, '+').replace(/_/g, '/');
        var jsonPayload = decodeURIComponent(atob(base64).split('').map(function (c) {
            return '%' + ('00' + c.charCodeAt(0).toString(16)).slice(-2);
        }).join(''));
        return JSON.parse(jsonPayload);
    }

    function extractToken(name) {
        var hash = window.location.hash.substr(1);
        var startIndex = hash.indexOf(name + "=");
        if (startIndex >= 0) {
            startIndex += name.length + 1;
            var endIndex = hash.indexOf("&", startIndex);
            if (endIndex < 0) endIndex = hash.length;
            return hash.substr(startIndex, endIndex - startIndex);
        }
        else {
            return null;
        }
    }

    function checkAccessToken(oauthUrl) {
        var error = extractToken("error");
        if (error == null) {
            var access_token = extractToken("access_token");
            if (access_token == null) {
                var pathparts = window.location.pathname.substr(1).split('/').map(x => decodeURIComponent(x));
                return {
                    pathparts: pathparts
                };
            }
            else {
                var pathname = decodeURIComponent(extractToken("state"));
                var pathparts = pathname.substr(1).split('/').map(x => decodeURIComponent(x));
                return {
                    pathparts: pathparts,
                    access_token: access_token,
                    jwt: parseJwt(access_token)
                };
            }
        }
        else {
            var pathname = decodeURIComponent(extractToken("state"));
            var pathparts = pathname.substr(1).split('/').map(x => decodeURIComponent(x));
            var error_description = extractToken("error_description");
            return {
                pathparts: pathparts,
                error: decodeURIComponent(error),
                error_description: (error_description != null) ? decodeURIComponent(error_description) : null
            };
        }
    }

    function login(oauthUrl) {
        window.location = oauthUrl + "&state=" + encodeURIComponent(window.location.pathname);
    }

    return {
        checkAccessToken: checkAccessToken,
        login: login
    };
}();
