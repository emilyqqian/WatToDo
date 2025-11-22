import {
    Box,
    Card,
    Typography,
    LinearProgress,
    Stack,
    Chip,
    Avatar,
} from '@mui/material'
import LocalFireDepartmentIcon from '@mui/icons-material/LocalFireDepartment'
import EmojiEventsIcon from '@mui/icons-material/EmojiEvents'

export default function UserStats({ user, userLevel = 1 }) {
    // Calculate XP progress to next level (assuming 100 XP per level)
    const xpPerLevel = 100
    const currentLevelXP = (userLevel - 1) * xpPerLevel
    const nextLevelXP = userLevel * xpPerLevel
    const xpInCurrentLevel = (user?.xp_points || 0) - currentLevelXP
    const xpProgress = Math.min(100, Math.max(0, (xpInCurrentLevel / xpPerLevel) * 100))

    return (
        <Card
            elevation={0}
            sx={{
                background: '#1a1f3a',
                backdropFilter: 'none',
                border: '3px solid rgba(255, 255, 255, 0.2)',
                borderRadius: '0px',
                p: 3,
                mb: 3,
                boxShadow: '4px 4px 0px rgba(0, 0, 0, 0.15)',
            }}
        >
            <Stack spacing={2.5}>
                {/* Header with Avatar and Name */}
                <Stack direction="row" spacing={2} alignItems="center">
                    <Avatar
                        sx={{
                            width: 56,
                            height: 56,
                            background: '#667eea',
                            fontSize: '1.5rem',
                            fontWeight: 'bold',
                        }}
                    >
                        {user?.username?.charAt(0).toUpperCase() || 'U'}
                    </Avatar>
                    <Box flex={1}>
                        <Typography variant="h6" fontWeight={700}>
                            {user?.username || 'Player'}
                        </Typography>
                        <Typography variant="body2" color="text.secondary">
                            ID: {user?.userId || '—'}
                        </Typography>
                    </Box>
                </Stack>

                {/* Level Badge */}
                <Box
                    sx={{
                        display: 'flex',
                        alignItems: 'center',
                        gap: 1,
                        p: 1.5,
                        background: '#667eea',
                        borderRadius: '0px',
                        border: '2px solid rgba(255, 255, 255, 0.3)',
                        boxShadow: '2px 2px 0px rgba(0, 0, 0, 0.1)',
                    }}
                >
                    <EmojiEventsIcon sx={{ color: '#FFB81C', fontSize: '1.5rem' }} />
                    <Typography variant="subtitle2" fontWeight={600}>
                        Level {userLevel}
                    </Typography>
                </Box>

                {/* XP Progress */}
                <Box>
                    <Stack
                        direction="row"
                        justifyContent="space-between"
                        sx={{ mb: 1 }}
                    >
                        <Typography variant="body2" fontWeight={600}>
                            Experience
                        </Typography>
                        <Typography variant="body2" color="text.secondary">
                            {user?.xp_points || 0} XP
                        </Typography>
                    </Stack>
                    <LinearProgress
                        variant="determinate"
                        value={xpProgress}
                        sx={{
                            height: 8,
                            borderRadius: '0px',
                            border: '1px solid rgba(255, 255, 255, 0.3)',
                            background: 'rgba(255, 255, 255, 0.3)',
                            '& .MuiLinearProgress-bar': {
                                background: '#667eea',
                                borderRadius: '0px',
                            },
                        }}
                    />
                    <Typography variant="caption" color="text.secondary" sx={{ mt: 0.5, display: 'block' }}>
                        {xpPerLevel - xpInCurrentLevel} XP to next level
                    </Typography>
                </Box>

                {/* Stats Row */}
                <Stack direction="row" spacing={1}>
                    <Chip
                        icon={<LocalFireDepartmentIcon />}
                        label={`${Math.floor((user?.xp_points || 0) / 50)} Streak`}
                        variant="outlined"
                        size="small"
                        sx={{
                            borderColor: 'rgba(255, 107, 107, 0.5)',
                            color: '#FF6B6B',
                        }}
                    />
                    <Chip
                        label={`${user?.xp_points || 0} Total XP`}
                        variant="filled"
                        size="small"
                        sx={{
                            background: 'rgba(102, 126, 234, 0.3)',
                        }}
                    />
                </Stack>
            </Stack>
        </Card>
    )
}
